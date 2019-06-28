import java.util.*
import kotlin.random.Random

// Test multiple deletions

fun main() {
    val nItems = 1000000
    val probs = Array(nItems) { Random.nextDouble() }
    val categorical = MutableCategorical<Int>(nItems)
    val categoricalRotate = MutableCategoricalWithRotation<Int>(nItems)

    categorical.createHuffmanTree(1..nItems, probs.asList())
    categoricalRotate.createHuffmanTree(1..nItems, probs.asList())

    val categories = TreeSet<Int>()
    for(i in 1..nItems) categories.add(i)
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