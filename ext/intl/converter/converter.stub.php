<?php

/** @generate-function-entries */

class UConverter
{
    public function __construct(?string $destination_encoding = null, ?string $source_encoding = null) {}

    /** @return string|false */
    public function convert(string $str, bool $reverse = false) {}

    /**
     * @param int $error
     * @return string|int|array|null
     */
    public function fromUCallback(int $reason, array $source, int $codePoint, &$error) {}

    /** @return array|false|null */
    public static function getAliases(string $name) {}

    /** @return array */
    public static function getAvailable() {}

    /** @return string|false|null */
    public function getDestinationEncoding() {}

    /** @return int|false|null */
    public function getDestinationType() {}

    /** @return int */
    public function getErrorCode() {}

    /** @return string|null */
    public function getErrorMessage() {}

    /** @return string|false|null */
    public function getSourceEncoding() {}

    /** @return int|false|null */
    public function getSourceType() {}

    /** @return array|null */
    public static function getStandards() {}

    /** @return string|false|null */
    public function getSubstChars() {}

    /** @return string */
    public static function reasonText(int $reason) {}

    /** @return bool */
    public function setDestinationEncoding(string $encoding) {}

    /** @return bool */
    public function setSourceEncoding(string $encoding) {}

    /** @return bool */
    public function setSubstChars(string $chars) {}

    /**
     * @param int $error
     * @return string|int|array|null
     */
    public function toUCallback(int $reason, string $source, string $codeUnits, &$error) {}

    /** @return string|false */
    public static function transcode(string $str, string $toEncoding, string $fromEncoding, ?array $options = null) {}
}
