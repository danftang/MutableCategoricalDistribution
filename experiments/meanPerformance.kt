import kotlin.math.ln
import kotlin.random.Random

fun main() {
    println("Distribution\tHuffman\t\tCategorical\tRotation\tCat ratio\tRotation ratio")
    print("Uniform\t")
    calculatePerformance { Random.nextDouble() }

    print("Exponential")
    calculatePerformance  { -ln(1.0-Random.nextDouble()) }

    print("Resonance")
    calculatePerformance {
        val r = Random.nextDouble()
        if(r < 0.99) 1.0 else 1000.0
    }
}

fun calculatePerformance(probGenerator : ()->Double) {
    val nItems = 100000
    val probs = Array(nItems) {probGenerator()}
    val categorical = MutableCategoricalMap<Int>(nItems)
    val categoricalRotate = MutableCategoricalMapWithRotation<Int>(nItems)

    categorical.createHuffmanTree(0..2*nItems step 2, probs.asList())
    categoricalRotate.createHuffmanTree(0..2*nItems step 2, probs.asList())

    val nMods = 1000
    val burnIn = 500
    var huffmanTotal = 0.0
    var catTotal = 0.0
    var rotateTotal = 0.0
    var count = 0
    for(m in 1..nMods) {
        for(q in 1..500) {
            var toModify = Random.nextInt(nItems)
            if (Random.nextInt(3) < 2) {
                val newVal = probGenerator()
                categorical[toModify] = newVal
                categoricalRotate[toModify] = newVal
            } else {
                while(categorical.remove(toModify) == null) toModify = (toModify + 1).rem(nItems)
                categoricalRotate.remove(toModify)
            }
        }
        if(m > burnIn) {
            val huffman = calcHuffmanLength(categorical.values)
            val cat = categorical.calcHuffmanLength()
            val rotate = categoricalRotate.calcHuffmanLength()
            huffmanTotal += huffman
            catTotal += cat
            rotateTotal += rotate
            count += 1
        }
    }

    println("\t\t%.4f\t\t%.4f\t\t%.4f\t\t%.4f\t\t%.4f".format(
        huffmanTotal/count,
        catTotal/count,
        rotateTotal/count,
        catTotal/huffmanTotal,
        rotateTotal/huffmanTotal))
}
