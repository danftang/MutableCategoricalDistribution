import kotlin.random.Random

fun main() {
//    testAddRemove()
//    testChoose()
//    testHuffmanLength()
    testMutability()
}

fun testHuffmanLength() {
    val myChoose = MutableCategorical<Int>()

    myChoose.createHuffmanTree(listOf(1,2),listOf(0.1,0.2))

    val nTests = 10
    var total = 0.0
    var huffTotal = 0.0
    var chooseTotal = 0.0
    for(n in 1..nTests) {
        val nItems = 1024*1024//Random.nextInt(1000) + 2
//        val probs = Array(nItems) { -ln(1.0-Random.nextDouble()) }
//        val probs = Array(nItems) { Random.nextDouble() }
//        val probs = Array(nItems) { exp(Random.nextDouble()*ln(100000.0)) }
        val probs = Array(nItems) {
            val r = Random.nextDouble()
            if(r < 0.99) r else 10000.0 }
        probs.forEachIndexed { i, p ->
            myChoose[i] = p
        }
        val huffman = calcHuffmanLength(probs.asList())
        val choose = myChoose.calcHuffmanLength()
        val ratio = choose/huffman
        total += ratio
        huffTotal += huffman
        chooseTotal += choose
//        println("$n $ratio $huffman $sample $total")
        myChoose.clear()
    }

    println("average ratio = ${total/nTests}")
    println("average huff = ${huffTotal/nTests}")
    println("average choo = ${chooseTotal/nTests}")
    println("totals ratio = ${chooseTotal/huffTotal}")

}

fun testMutability() {
    val nItems = 100000
    val probGenerator : ()->Double = { spikeyNoise() }
    val probs = Array(nItems) {probGenerator()}
    val myChoose = MutableCategorical<Int>(nItems)
    myChoose.createHuffmanTree(1..nItems, probs.asList())
    println(myChoose.calcHuffmanLength()/ calcHuffmanLength(myChoose.values))

    var total = 0.0
    val nMods = 50
    for(m in 1..nMods) {
        for(q in 1..1000) {
            val toModify = Random.nextInt(nItems)
            if (Random.nextInt(3) < 2) {
                myChoose[toModify] = probGenerator()
            } else {
                myChoose.remove(toModify)
            }
        }
        val huffman = calcHuffmanLength(myChoose.values)
        val choose = myChoose.calcHuffmanLength()
        val ratio = choose/huffman
        total += ratio
        println(ratio)
    }
}

fun testChoose() {
    val myChoose = MutableCategorical<Int>()

    myChoose[0] = 0.6
    myChoose[1] = 0.2
    myChoose[2] = 0.1
    myChoose[3] = 0.1

    val count = Array(4) {0}
    val nSamples = 10000
    for(i in 1..nSamples) {
        count[myChoose.sample()]++
    }
    for(i in 0 until count.size) {
        println("$i -> ${count[i]*1.0/nSamples}")
        println("${myChoose[i]}")
    }


}

fun testAddRemove() {
    val myChoose = MutableCategorical<Int>()

    myChoose[0] = 0.8
    myChoose[1] = 0.2
    myChoose[2] = 0.1
    myChoose[3] = 0.1
    myChoose[4] = 0.5

    myChoose[0] = 0.6

    println(myChoose)
    myChoose.remove(4)
    println(myChoose)
    myChoose.remove(1)
    println(myChoose)
    myChoose.remove(3)
    println(myChoose)
    myChoose.remove(0)
    println(myChoose)
    myChoose.remove(2)
    println(myChoose)

}

fun spikeyNoise() : Double {
    val r = Random.nextDouble()
    return if(r < 0.99) r else 10000.0
}
