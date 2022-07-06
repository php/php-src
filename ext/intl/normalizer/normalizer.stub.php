<?php

/** @generate-function-entries */

class Normalizer
{
    /**
     * @return string|false
     * @alias normalizer_normalize
     */
    public static function normalize(string $string, int $form = Normalizer::FORM_C) {}

    /**
     * @return bool
     * @alias normalizer_is_normalized
     */
    public static function isNormalized(string $string, int $form = Normalizer::FORM_C) {}

#if U_ICU_VERSION_MAJOR_NUM >= 56
    /**
     * @return string|null
     * @alias normalizer_get_raw_decomposition
     */
    public static function getRawDecomposition(string $string, int $form = Normalizer::FORM_C) {}
#endif
}
