import java.util.*
import kotlin.random.Random

// Test multiple deletions

fun main() {
    val nItems = 1000000
    val probs = Array(nItems) { Random.nextDouble() }
    val categorical = MutableCategorical(probs.asSequence().mapIndexed { i, p -> Pair(i, p) }.asIterable())
    val categoricalRotate = MutableCategoricalWithRotation(probs.asSequence().mapIndexed { i, p -> Pair(i, p) }.asIterable())
    val categories = TreeSet(categorical.keys)


    val batchsize = 500
    while (categorical.size > 1024) {
        val randCategory = Random.nextInt(nItems)
        val catToDelete = categories.ceiling(randCategory) ?: categories.floor(randCategory)
        categories.remove(catToDelete)
        categorical.remove(catToDelete)
        categoricalRotate.remove(catToDelete)
    }
    val huffman = calcHuffmanLength(categorical.values)
    val cat = categorical.calcHuffmanLength()
    val rotate = categoricalRotate.calcHuffmanLength()

    println("$huffman ${cat / huffman} ${rotate / huffman}")
}