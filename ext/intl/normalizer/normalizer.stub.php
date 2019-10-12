<?php

class Normalizer
{
    /** @return string|false */
    public static function normalize(string $input, int $form = Normalizer::FORM_C) {}

    /** @return bool */
    public static function isNormalized(string $input, int $form = Normalizer::FORM_C) {}

#if U_ICU_VERSION_MAJOR_NUM >= 56
    /** @return string|null */
    public static function getRawDecomposition(string $input, int $form = Normalizer::FORM_C) {}
#endif
}

function normalizer_normalize(string $input, int $form = Normalizer::FORM_C): string|false {}

function normalizer_is_normalized(string $input, int $form = Normalizer::FORM_C): bool {}

#if U_ICU_VERSION_MAJOR_NUM >= 56
function normalizer_get_raw_decomposition(string $input, int $form = Normalizer::FORM_C): ?string {}
#endif
