# A Mutable Categorical Distribution

Here are some C++ and Kotlin classes that can be used to efficiently sample from a *categorical distribution* (sometimes called an *empirical distribution*) which is a probability distribution over a finite number of categories $C_0...C_N$ where each category is associated with a weight $w_i$ and the probability of the $i^{th}$ category is

$$P(C_i) = \frac{w_i}{\sum_j w_j}$$

Unlike existing library implementations, these classes allow the weights to be modified in O(log(n)) time (instead of O(n) time), while maintining sampling in O(log(n)) time. If you're happy for your categories to be labelled by the integers 0...N then use `MutableCategoricalArray`, whereas if you want to label the categories with objects of some arbitrary class then use `MutableCategorical` or `MutableCategoricalMap`. The Arrray class is faster than the Map, so use that where possible (only the highest index category can be removed from a `MutableCategoricalArray` but the weight of any category can be set to zero to effectively remove it).
 
 A `MutableCategoricalArray` has a similar interface to an array of Doubles, with the addition of a `myObj.sample()` method in Kotlin or a myObj(generator) in C++, which returns an integer, `i`, with probability `myObj[i]`. So, for example, to simulate a fair coin toss (returning either 0 or 1 with probability 0.5) in Kotlin the code would be:
```kotlin
    val categorical = MutableCategoricalArray(2) { 0.5 }
    int result = categorical.sample()
```
while in C++ the code would be
```C++
    MutableCategoricalArray categorical { 0.5, 0.5 };
    std::default_random_generator rng;
    int result = categorical(rng);
}
```
Weights can be queried in amortized constant time using `myObj[i]` and modified using `myObj[i] = someNewProbability` in O(log(n)) time. Weights are normalised internally so need not sum to one. To query the normalised probability use `myObj.P(i)`. If you want to change all probabilities, this can be done in O(n) time using the `setAll` method. There's also a `sum()` method which returns the sum of all numbers in the array in O(1) time.

The Kotlin `MutableCategoricalMap` class implements the `MutableMap<CATEGORY,Double>` interface, so just treat it like a map from category values to category probabilities. Categories can be added and deleted and their probabilities modified, all in O(log(n)) time. Sampling is also O(log(n)) time.

So, for example, to create a fair coin flip...
```kotlin
enum class Coin {Heads, Tails}

    val categorical = MutableCategorical<Coin>()
    categorical[Coin.Heads] = 0.5
    categorical[Coin.Tails] = 0.5
    Coin result = categorical.sample()
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

There are two C++ classes that allow arbitrary labeling of categories: `MutableCategorical` and `MutableCategoricalMap`. These are similar to the Kotlin `MutableCategoricalMap` but use iterators instead of category values to identify categories, in order to achieve better performance. `MutableCategorical` uses a `MutableCategoricalArray` as its underlying data structure whereas `MutableCategoricalMap` uses a fully specified binary tree. If you're going to be modifying the distribution between each draw, use `MutableCategorical`, but if you intend to take many draws between modification, use `MutableCategoricalMap`.

The [accompanying paper](./paper.pdf) describes the algorithm used in `MutableCategoricalMap` along with a demonstration of its efficiency in practice. If you're concerned about worst-case performance, there's a class `MutableCategoricalWithRotation` in the `experiments/` folder. This version performs tree rotations on addition and deletion to ensure the worst case remains O(log(n)). However, as noted in the paper, the improvement in practice is expected to be small so I recommend using `MutableCategorical`.
