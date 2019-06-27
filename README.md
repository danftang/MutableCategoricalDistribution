# A Mutable Categorical Distribution

With the recent rise in popularity of probabilistic programming libraries there is a need to develop efficient algorithms to work with probability distributions. Here we present a Kotlin class `MutableCategorical` that efficiently implements the *categorical distribution* (sometimes called an *empirical distribution*) which is a probability distribution over a finite number of states where the probability of each state is specified as a parameter.

Unlike existing implementations, this one allows categories to be added and deleted and the probabilities of the categories can be modified, all in O(log(n)) time instead of O(n) time.

The class implements the `Map<CATEGORY,Double>` interface and overrides the `[]` operator, so just treat it like a map from category values to category probabilities. To take a sample, just call the `sample` method. So, for example, to create a fair coin flip...
```kotlin
enum class Coin {Heads, Tails}

fun toss() : Coin {
    val categorical = MutableCategorical<Coin>()
    categorical[Coin.Heads] = 0.5
    categorical[Coin.Tails] = 0.5
    return categorical.sample()
}
```

The [accompanying paper](./paper.pdf) describes the algorithm along with a demonstration of its efficiency in practice. If you're concerned about worst-case performance, we also provide a class `MutableCategoricalWithRotation` in the `experiments/` folder. This version performs tree rotations on addition and deletion to ensure the tree doesn't get inefficient. However, as noted in the paper, the improvement in practice is expected to be small.