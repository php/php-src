<?php

/** @generate-class-entries */

class Normalizer
{
    /**
     * @cvalue NORMALIZER_FORM_D
     */
    public const int FORM_D = UNKNOWN;
    /**
     * @cvalue NORMALIZER_NFD
     */
    public const int NFD = UNKNOWN;
    /**
     * @cvalue NORMALIZER_FORM_KD
     */
    public const int FORM_KD = UNKNOWN;
    /**
     * @cvalue NORMALIZER_NFKD
     */
    public const int NFKD = UNKNOWN;
    /**
     * @cvalue NORMALIZER_FORM_C
     */
    public const int FORM_C = UNKNOWN;
    /**
     * @cvalue NORMALIZER_NFC
     */
    public const int NFC = UNKNOWN;
    /**
     * @cvalue NORMALIZER_FORM_KC
     */
    public const int FORM_KC = UNKNOWN;
    /**
     * @cvalue NORMALIZER_NFKC
     */
    public const int NFKC = UNKNOWN;
#if U_ICU_VERSION_MAJOR_NUM >= 56
    /**
     * @cvalue NORMALIZER_FORM_KC_CF
     */
    public const int FORM_KC_CF = UNKNOWN;
    /**
     * @cvalue NORMALIZER_NFKC_CF
     */
    public const int NFKC_CF = UNKNOWN;
#endif

    /**
     * @tentative-return-type
     * @alias normalizer_normalize
     */
    public static function normalize(string $string, int $form = Normalizer::FORM_C): string|false {}

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
