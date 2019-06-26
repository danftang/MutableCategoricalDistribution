
// A fair coin toss

enum class Coin {Heads, Tails}

fun toss() : Coin {
    val categorical = MutableCategorical<Coin>()
    categorical[Coin.Heads]  = 0.5
    categorical[Coin.Tails] = 0.5
    return categorical.sample()
}

fun main() {
    println(toss())
}