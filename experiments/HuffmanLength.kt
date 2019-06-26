import java.util.*

fun calcHuffmanLength(probs : Collection<Double>) : Double {
    val heap = PriorityQueue<Double>(probs.size)

    probs.forEach {
        heap.add(it)
    }

    var totalLength = 0.0
    while(heap.size > 1) {
        val first = heap.poll()
        val second = heap.poll()
        val sum = first + second
        totalLength += sum
        heap.add(sum)
    }

    val total = heap.poll()

    return totalLength/total
}