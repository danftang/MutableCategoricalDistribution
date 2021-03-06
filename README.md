# A Mutable Categorical Distribution

Here are two Kotlin classes `MutableCategoricalMap` and `MutableCategoricalArray` that can be used to efficiently sample from a *categorical distribution* (sometimes called an *empirical distribution*) which is a probability distribution over a finite number of objects where the probability of each object is specified as a parameter.

Unlike existing implementations, these classes allow the probabilities to be modified in O(log(n)) time (instead of O(n) time), while maintining sampling in O(log(n)) time. If the number of objects is fixed, use `MutableCategoricalArray`, whereas if you want to add/delete objects use `MutableCategoricalMap`.
 
 A `MutableCategoricalArray` has a similar interface to an array of Doubles, with the addition of a `myObj.sample()` method, which returns an integer, `i`, with probability `myObj[i]`. So, for example the code:
```kotlin
fun toss() : Int {
    val categorical = MutableCategoricalArray(2) { 0.5 }
    return categorical.sample()
}
```
would simulate a fair coin toss, returning either 0 or 1 with probability 0.5. Probabilities can be modified using `myObj[i] = someNewProbability`. Probabilities are normalised internally so need not sum to one. If you want to change all probabilities at once, this can be done with constant-time complexity per modification using the `setAll` method. There's also a `sum()` method which returns the sum of all numbers in the array in O(1) time.

The `MutableCategoricalMap` class implements the `MutableMap<CATEGORY,Double>` interface, so just treat it like a map from category values to category probabilities. Categories can be added and deleted and their probabilities modified, all in O(log(n)) time. Sampling is also O(log(n)) time.

So, for example, to create a fair coin flip...
```kotlin
enum class Coin {Heads, Tails}

fun toss() : Coin {
    val categorical = MutableCategorical<Coin>()
    categorical[Coin.Heads] = 0.5
    categorical[Coin.Tails] = 0.5
    return categorical.sample()
}
```

Internally, this is represented as a sum tree. You can initialise a `MutableCategorical` to be an (optimal) Huffman tree using the `createHuffmanTree` method; this takes O(nlog(n)) time. Alternatively, you can initialise it to a minimal depth tree (which isn't quite optimal) in O(n) time by calling `createBinaryTree`. So, for example
```kotlin
val categorical = MutableCategorical<Int>()
categorical.createHuffmanTree(1..4, listOf(0.1,0.2,0.3,0.4))
```
would create an optimal categorical with the integers 1 to 4 having probabilities 0.1, 0.2, 0.3 and 0.4 respectively.

Alternatively, you can create a new instances with the `mutableCategoricalOf`. For example...
```kotlin
val categorical = mutableCategoricalOf(1 to 0.6, 2 to 0.4)
```
creates a `MutableCategoricalMap` initialised with a binary tree of the given mappings, and
```kotlin
val categorical = mutableCategoricalOf(0.6, 0.4)
```
creates a `MutableCategoricalArray` with the given probabilities.

The [accompanying paper](./paper.pdf) describes the algorithm used in `MutableCategoricalMap` along with a demonstration of its efficiency in practice. If you're concerned about worst-case performance, there's a class `MutableCategoricalWithRotation` in the `experiments/` folder. This version performs tree rotations on addition and deletion to ensure the worst case remains O(log(n)). However, as noted in the paper, the improvement in practice is expected to be small so I recommend using `MutableCategorical`.