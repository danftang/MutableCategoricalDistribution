import java.util.*
import kotlin.NoSuchElementException
import kotlin.collections.AbstractMap
import kotlin.collections.AbstractSet
import kotlin.collections.HashMap
import kotlin.random.Random

class MutableCategorical<T>: AbstractMap<T,Double> {
    private var sumTreeRoot: SumTreeNode<T>? = null
    private val leafNodes = HashMap<T,SumTreeNode<T>>()


    constructor()


    constructor(probs : Map<T,Double>) {
        createHuffmanTree(probs.asSequence().map {SumTreeNode(null,it.key, it.value)}.asIterable(), probs.size)
    }


    constructor(probs : Iterable<Pair<T,Double>>, initialSize : Int = 1024) {
        createHuffmanTree(probs.asSequence().map {SumTreeNode(null,it.first, it.second)}.asIterable(), initialSize)
    }


    private fun createHuffmanTree(probs : Iterable<SumTreeNode<T>>, initialSize : Int = 1024) {
        val heap = PriorityQueue<SumTreeNode<T>>(initialSize) {i, j -> i.value.compareTo(j.value)}

        probs.forEach {
            heap.add(it)
            leafNodes[it.key] = it
        }

        while(heap.size > 1) {
            val first = heap.poll()
            val second = heap.poll()
            val parent = SumTreeNode(null, first, second)
            heap.add(parent)
        }
        sumTreeRoot = heap.poll()
    }


    override val entries: Set<Map.Entry<T, Double>>
        get() = EntrySet(leafNodes.values)

    override val size: Int
        get() = leafNodes.size

    override operator fun get(key : T) : Double {
        return leafNodes.getValue(key).value
    }


    fun put(item : T, probability : Double) {
        set(item, probability)
    }


    operator fun set(item : T, probability : Double) {
        val root = sumTreeRoot
        if(root == null) {
            val newNode = SumTreeNode(null, item, probability)
            sumTreeRoot = newNode
            leafNodes[item] = newNode
            return
        }
        val existingNode = leafNodes[item]
        if(existingNode == null) {
            val newNode = SumTreeNode(null, item, probability)
            sumTreeRoot = root.add(newNode)
            leafNodes[item] = newNode
            return
        }
        val newRoot = existingNode.remove()
        existingNode.value = probability
        if(newRoot == null) {
            sumTreeRoot = existingNode
            return
        }
        sumTreeRoot = newRoot.add(existingNode)
    }


    fun remove(item :T) : Boolean {
        val node = leafNodes.remove(item) ?: return false
        sumTreeRoot = node.remove()
        return true
    }


    fun sample(sum : Double) : T? {
        return sumTreeRoot?.find(sum)?.key
    }


    fun sample() : T {
        val root = sumTreeRoot ?: throw(NoSuchElementException())
        return sample(Random.nextDouble()*root.value)!!
    }


    fun clear() {
        leafNodes.clear()
        sumTreeRoot = null
    }


    fun calcHuffmanLength() : Double {
        return (sumTreeRoot?.calcHuffmanLength()?:0.0) / (sumTreeRoot?.value?:1.0)
    }


    class SumTreeNode<T> : Map.Entry<T, Double> {
        var parent: SumTreeNode<T>?
        override var value: Double
        private var childKeyUnion : Any?
        private var nullableRChild: SumTreeNode<T>?

        var lChild: SumTreeNode<T>
            get() {
                if(isLeaf()) throw(java.lang.IllegalStateException("Can't get child of a leaf node"))
                return childKeyUnion as SumTreeNode<T>
            }
            private set(i) {
                childKeyUnion = i
            }

        var rChild : SumTreeNode<T>
            get() = nullableRChild?:throw(java.lang.IllegalStateException("Can't get child of a leaf node"))
            private set(i) {
                nullableRChild = i
            }

        override var key: T
            get() {
                if(!isLeaf()) throw(java.lang.IllegalStateException("Can't read key from a non-leaf node"))
                return childKeyUnion as T
            }
            private set(i) {
                childKeyUnion = i
            }

        constructor(parent: SumTreeNode<T>?, item: T, probability: Double) {
            this.parent = parent
            this.value = probability
            this.childKeyUnion = item
            this.nullableRChild = null
        }

        constructor(parent : SumTreeNode<T>?, child1: SumTreeNode<T>, child2: SumTreeNode<T>) {
            this.parent = parent
            if(child1.value > child2.value) {
                this.childKeyUnion = child1
                this.nullableRChild = child2
            } else {
                this.childKeyUnion = child2
                this.nullableRChild = child1
            }
            child1.parent = this
            child2.parent = this
            this.value = child1.value + child2.value
        }

        fun find(sum : Double) : SumTreeNode<T>? {
            if(isLeaf()) {
                if(sum > value || sum < 0.0) return null
                return this
            }
            if(sum <= lChild.value) return lChild.find(sum)
            return rChild.find(sum - lChild.value)
        }

        // returns the root node
        fun updateValuesToRoot() : SumTreeNode<T> {
            updateValue()
            return parent?.updateValuesToRoot()?:this
        }

        fun updateValue() {
            if(!isLeaf()) {
                value = lChild.value + rChild.value
                if(lChild.value < rChild.value) {
                    val tmp = rChild
                    rChild = lChild
                    lChild = tmp
                }
            }
        }

        fun add(newNode: SumTreeNode<T>) : SumTreeNode<T> {
            if(isLeaf() || value <= newNode.value) { // add right here
                val oldParent = parent
                val newParent = SumTreeNode(parent, this, newNode)
                oldParent?.swapChild(this, newParent)
                newParent.parent?.updateValuesToRoot()
                return newParent
            }
            rChild.add(newNode)
            return this
        }

        // removes this and returns the root node
        fun remove() : SumTreeNode<T>? {
            if(!isLeaf()) throw(IllegalArgumentException("Trying to remove an internal node"))
            val p = parent?:return null
            val sib = p.otherChild(this)
            p.parent?.swapChild(p, sib)
            sib.parent = p.parent
            return sib.parent?.updateValuesToRoot()?:sib
        }


        fun swapChild(oldChild: SumTreeNode<T>, newChild: SumTreeNode<T>) {
            if(oldChild == lChild) {
                lChild = newChild
                return
            } else if(oldChild == rChild) {
                rChild = newChild
                return
            }
            throw(IllegalStateException("trying to swap a child that isn't a child"))
        }


        fun otherChild(firstChild : SumTreeNode<T>) : SumTreeNode<T> {
            return when (firstChild) {
                lChild -> rChild
                rChild -> lChild
                else -> throw(NoSuchElementException())
            }
        }

        fun isLeaf() : Boolean {
            return nullableRChild == null
        }

        fun calcHuffmanLength() : Double {
            if(isLeaf()) return 0.0
            return value + lChild.calcHuffmanLength() + rChild.calcHuffmanLength()
        }
    }

    class EntrySet<T>(private val leafNodeEntries: Collection<SumTreeNode<T>>) : AbstractSet<Map.Entry<T,Double>>() {
        override val size: Int
            get() = leafNodeEntries.size
        override fun iterator() = leafNodeEntries.iterator()
    }

    override fun toString() : String {
        var s = "(  "
        for(item in entries) {
            s += "${item.key}->${item.value}  "
        }
        s += ")"
        return s
    }

}
