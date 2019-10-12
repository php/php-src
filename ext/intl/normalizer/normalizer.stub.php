<?php

class Normalizer
{
    /** @return string|false */
    public static function normalize(string $input, int $form = Normalizer::FORM_C) {}

    /** @return bool */
    public static function isNormalized(string $input, int $form = Normalizer::FORM_C) {}

    /** @return string|null */
    public static function getRawDecomposition(string $input, int $form = Normalizer::FORM_C) {}
}

/** @return string|false */
function normalizer_normalize(string $input, int $form = Normalizer::FORM_C) {}

/** @return bool */
function normalizer_is_normalized(string $input, int $form = Normalizer::FORM_C) {}

/** @return string|null */
function normalizer_get_raw_decomposition(string $input, int $form = Normalizer::FORM_C) {}
