# A Mutable Categorical Distribution

With the recent rise in popularity of probabilistic programming libraries there is a need to develop efficient algorithms to work with probability distributions. This is a Kotlin class `MutableCategorical` that efficiently implements the *categorical distribution* (sometimes called an *empirical distribution*) which is a probability distribution over a finite number of states where the probability of each state is specified as a parameter.

Unlike existing implementations, this class allows categories to be added and deleted and the probabilities of the categories to be modified, all in O(log(n)) time instead of O(n) time. The algorithm is based on Huffman coding and we present new algorithms to efficiently modify the coding tree.

The class implements the `MutableMap<CATEGORY,Double>` interface, so just treat it like a map from category values to category probabilities. To take a sample, call the `sample` method. So, for example, to create a fair coin flip...
```kotlin
enum class Coin {Heads, Tails}

fun toss() : Coin {
    val categorical = MutableCategorical<Coin>()
    categorical[Coin.Heads] = 0.5
    categorical[Coin.Tails] = 0.5
    return categorical.sample()
}
```

You can initialise a `MutableCategorical` to be an (optimal) Huffman tree using the `createHuffmanTree` method; this takes O(nlog(n)) time. Alternatively, you can initialise it to a minimal depth tree (which isn't quite optimal) in O(n) time by calling `createBinaryTree`. So, for example
```kotlin
val categorical = MutableCategorical<Int>()
categorical.createHuffmanTree(1..4, listOf(0.1,0.2,0.3,0.4))
```
would create an optimal categorical with the integers 1 to 4 having probabilities 0.1, 0.2, 0.3 and 0.4 respectively.

The [accompanying paper](./paper.pdf) describes the algorithm along with a demonstration of its efficiency in practice. If you're concerned about worst-case performance, we also provide a class `MutableCategoricalWithRotation` in the `experiments/` folder. This version performs tree rotations on addition and deletion to ensure the tree remains efficient. However, as noted in the paper, the improvement in practice is expected to be small so I recommend using `MutableCategorical`.