class MutableCategoricalMapWithRotation<T> : MutableCategoricalMap<T> {
    constructor() : super()
    constructor(initialCapacity: Int) : super(initialCapacity)

    override fun createLeaf(parent : InternalNode<T>?, category : T, probability : Double) =
        LeafNodeWithRotation(parent, category, probability)


    override fun createNode(parent : InternalNode<T>?, child1: SumTreeNode<T>, child2: SumTreeNode<T>) =
        InternalNodeWithRotation(parent, child1, child2)


    class LeafNodeWithRotation<T> : LeafNode<T> {
        constructor(parent: InternalNode<T>?, item: T, probability: Double) : super(parent, item, probability)

        override fun createInternalNode(parent : InternalNode<T>?, child1: SumTreeNode<T>, child2: SumTreeNode<T>) =
            InternalNodeWithRotation(parent, child1, child2)
    }


    class InternalNodeWithRotation<T> : InternalNode<T> {
        constructor(parent: InternalNode<T>?, child1: SumTreeNode<T>, child2: SumTreeNode<T>) :
                super(parent, child1, child2)


        fun rotate() {
            val lChild = leftChild as InternalNodeWithRotation<T>
            parent?.swapChild(this, lChild)
            lChild.parent = parent
            parent = lChild
            lChild.rightChild.parent = this
            leftChild = lChild.rightChild
            parent!!.rightChild = this
            super.updateSum()
        }


        override fun updateSum() {
            super.updateSum()
            val lChild = leftChild
            if(lChild is InternalNodeWithRotation<T> && rightChild.value < lChild.leftChild.value) rotate()
        }
    }
}