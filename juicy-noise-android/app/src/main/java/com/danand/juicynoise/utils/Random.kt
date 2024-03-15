package com.danand.juicynoise.utils

fun <T> weightedChoice(options: Map<T, Double>): T {
    val totalWeight = options.values.sum()
    val normalizedOptions = options.mapValues { it.value / totalWeight }

    var random = Math.random()

    for ((option, weight) in normalizedOptions) {
        if (random < weight) {
            return option
        }

        random -= weight
    }
    throw IllegalStateException("No option selected")
}
