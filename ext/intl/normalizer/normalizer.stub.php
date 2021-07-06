<?php

/** @generate-class-entries */

class Normalizer
{
    /**
     * @tentative-return-type
     * @alias normalizer_normalize
     */
    public static function normalize(string $string, int $form = Normalizer::FORM_C): string|false|null {}

    /**
     * @tentative-return-type
     * @alias normalizer_is_normalized
     */
    public static function isNormalized(string $string, int $form = Normalizer::FORM_C): bool {}

#if U_ICU_VERSION_MAJOR_NUM >= 56
    /**
     * @tentative-return-type
     * @alias normalizer_get_raw_decomposition
     */
    public static function getRawDecomposition(string $string, int $form = Normalizer::FORM_C): ?string {}
#endif
}
