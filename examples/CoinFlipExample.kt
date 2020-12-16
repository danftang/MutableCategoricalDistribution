
// A fair coin toss

enum class Coin {Heads, Tails}

fun mapCoinToss() : Coin {
    val categorical = MutableCategoricalMap<Coin>()
    categorical[Coin.Heads]  = 0.5
    categorical[Coin.Tails] = 0.5
    return categorical.sample()
}

fun arrayCoinToss() : Coin {
    val categorical = MutableCategoricalArray(2) { 0.5 }
    return if(categorical.sample() == 0) Coin.Heads else Coin.Tails
}


fun main() {
    println(mapCoinToss())
}