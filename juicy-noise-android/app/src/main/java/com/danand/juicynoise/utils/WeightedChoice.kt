package com.danand.juicynoise.utils

import kotlin.random.Random

class WeightedChoice<T>(private val items: List<Pair<T, Double>>) {

    private val cumulativeProbabilities: List<Double> = calculateCumulativeProbabilities()

    private fun calculateCumulativeProbabilities(): List<Double> {
        var cumulativeProbability = 0.0
        return items.map { (_, weight) ->
            cumulativeProbability += weight
            cumulativeProbability
        }
    }

    fun next(): T {
        val randomValue = Random.nextDouble(0.0, cumulativeProbabilities.last())
        val index = cumulativeProbabilities.indexOfFirst { it >= randomValue }

        return items[index].first
    }
}
