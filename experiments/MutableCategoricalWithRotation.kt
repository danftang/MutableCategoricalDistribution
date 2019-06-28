import java.util.*
import kotlin.NoSuchElementException
import kotlin.collections.AbstractMap
import kotlin.collections.AbstractSet
import kotlin.collections.HashMap
import kotlin.random.Random

class MutableCategoricalWithRotation<T> : AbstractMap<T,Double> {
    private var sumTreeRoot: SumTreeNode<T>? = null
    private val leafNodes: HashMap<T,SumTreeNode<T>>

    override val entries: Set<Map.Entry<T, Double>>
        get() = EntrySet(leafNodes.values)

    override val size: Int
        get() = leafNodes.size


    constructor() {
        leafNodes = HashMap()
    }


    constructor(initialCapacity : Int) {
        leafNodes = HashMap(initialCapacity)
    }


    // Sets this to be the Huffman tree of the given categories with the given probabilities
    // This creates an optimally efficient tree but runs in O(n log(n)) time as the entries
    // need to be sorted
    fun createHuffmanTree(categories : Iterable<T>, probabilities : Iterable<Double>, initialCapacity : Int = -1) {
        val heapComparator = Comparator<SumTreeNode<T>> {i, j -> i.value.compareTo(j.value)}
        if (initialCapacity > 0)
            createTree(categories, probabilities, PriorityQueue(initialCapacity, heapComparator))
        else
            createTree(categories, probabilities, PriorityQueue(calcCapacity(categories, probabilities), heapComparator))
    }


    // Sets this to be a binary tree of the given categories with the given probabilities
    // This creates a tree with minimal total depth and runs in O(n) time
    fun createBinaryTree(categories : Iterable<T>, probabilities : Iterable<Double>, initialCapacity : Int = -1) {
        if (initialCapacity > 0)
            createTree(categories, probabilities, ArrayDeque(initialCapacity))
        else
            createTree(categories, probabilities, ArrayDeque(calcCapacity(categories, probabilities)))
    }


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


    private fun <Q : Queue<SumTreeNode<T>>> createTree(categories : Iterable<T>, probabilities : Iterable<Double>, heap : Q) {
        val category = categories.iterator()
        val probability = probabilities.iterator()

        clear()
        while(category.hasNext() && probability.hasNext()) {
            val newNode = SumTreeNode(null, category.next(), probability.next())
            heap.add(newNode)
            leafNodes[newNode.key] = newNode
        }

        while(heap.size > 1) {
            val first = heap.poll()
            val second = heap.poll()
            val parent = SumTreeNode(null, first, second)
            heap.add(parent)
        }
        sumTreeRoot = heap.poll()
    }


    private fun calcCapacity(categories : Iterable<T>, probabilities : Iterable<Double>) : Int {
        return when {
            categories is Collection<T> -> categories.size
            probabilities is Collection<Double> -> probabilities.size
            else -> 1024
        }
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
                newParent.rebalance()
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
            sib.parent?.updateValue()
            return sib.parent?.rebalance()?:sib
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

        // rotates this and its ancestors
        // if necessary
        // returns the root node
        fun rebalance() : SumTreeNode<T> {
            updateValue()
            if(lChild.isLeaf()) {
                return parent?.rebalance()?:this
            }
            if(rChild.value >= lChild.lChild.value) {
                return parent?.rebalance()?:this
            }
            val newRoot = lChild
            rotate()
            return newRoot.parent?.rebalance()?:newRoot
        }


        fun rotate() {
            parent?.swapChild(this, lChild)
            lChild.parent = parent
            parent = lChild
            lChild.rChild.parent = this
            lChild = lChild.rChild
            parent!!.rChild = this
            this.updateValue()
            parent!!.updateValue()
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
