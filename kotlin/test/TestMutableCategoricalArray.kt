package test

import MutableCategoricalArray
import mutableCategoricalOf
import org.junit.Test
import kotlin.math.absoluteValue
import kotlin.math.sqrt
import kotlin.random.Random

class TestMutableCategoricalArray {


    @Test
    fun testOddCases() {
        //singleton
        assert(mutableCategoricalOf(0.1 ).sample() == 0)

        // zero probs
        assert(mutableCategoricalOf(0.0, 0.0, 1.0, 0.0, 0.0, 0.0 ).sample() == 2)

    }

    @Test
    fun testInitialization() {
        val myDistribution = mutableCategoricalOf(0.5,0.5)
        myDistribution.forEach {
            assert(it == 0.5)
            print("${it} ")
        }
        println()
        for(i in 0 until 100) {
            print("${myDistribution.sample()}")
        }
        println()

    }

    @Test
    fun testTriangular() {
        val N = 10
        val targetTriangular = DoubleArray(N) { it.toDouble() }
        val triangularDistribution = MutableCategoricalArray(N) { targetTriangular[it] }
        testDistribution(triangularDistribution, targetTriangular, 100000000)
    }

    @Test
    fun testModification() {
        val N = 5
        val targetDist = DoubleArray(N) { Random.nextDouble() }
        val testDist = MutableCategoricalArray(N) { targetDist[it] }
        for(i in 1..100) {
            testDistribution(testDist, targetDist, 10000000)
            val index = Random.nextInt(N)
            val newVal = Random.nextDouble()
            targetDist[index] = newVal
            testDist[index] = newVal
        }
        println()
    }


    fun testDistribution(dist: MutableCategoricalArray, targetDist: DoubleArray, nSamples: Int) {
        val targetSum = targetDist.sum()
        val targetProb = DoubleArray(targetDist.size) { targetDist[it]/targetSum }
        val histogram = IntArray(dist.size) { 0 }
        for(i in 1..nSamples) {
            histogram[dist.sample()] += 1
        }
        for(i in 0 until dist.size) {
            val sd = sqrt(nSamples*targetProb[i]*(1.0-targetProb[i]))
            val sampleError = targetProb[i]*nSamples - histogram[i]
            println("sample error = $sampleError Standard deviation = $sd")
            assert(sampleError.absoluteValue <= 4.0*sd)
        }
    }

}