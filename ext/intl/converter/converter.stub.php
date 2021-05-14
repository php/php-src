<?php

/** @generate-class-entries */

class UConverter
{
    public function __construct(?string $destination_encoding = null, ?string $source_encoding = null) {}

    /** @tentative-return-type */
    public function convert(string $str, bool $reverse = false): string|false {}

    /**
     * @param int $error
     * @tentative-return-type
     */
    public function fromUCallback(int $reason, array $source, int $codePoint, &$error): string|int|array|null {}

    /** @tentative-return-type */
    public static function getAliases(string $name): array|false|null {}

    /** @tentative-return-type */
    public static function getAvailable(): array {}

    /** @tentative-return-type */
    public function getDestinationEncoding(): string|false|null {}

    /** @tentative-return-type */
    public function getDestinationType(): int|false|null {}

    /** @tentative-return-type */
    public function getErrorCode(): int {}

    /** @tentative-return-type */
    public function getErrorMessage(): ?string {}

    /** @tentative-return-type */
    public function getSourceEncoding(): string|false|null {}

    /** @tentative-return-type */
    public function getSourceType(): int|false|null {}

    /** @tentative-return-type */
    public static function getStandards(): ?array {}

    /** @tentative-return-type */
    public function getSubstChars(): string|false|null {}

    /** @tentative-return-type */
    public static function reasonText(int $reason): string {}

    /** @tentative-return-type */
    public function setDestinationEncoding(string $encoding): bool {}

    /** @tentative-return-type */
    public function setSourceEncoding(string $encoding): bool {}

    /** @tentative-return-type */
    public function setSubstChars(string $chars): bool {}

    /**
     * @param int $error
     * @tentative-return-type
     */
    public function toUCallback(int $reason, string $source, string $codeUnits, &$error): string|int|array|null {}

    /** @tentative-return-type */
    public static function transcode(string $str, string $toEncoding, string $fromEncoding, ?array $options = null): string|false {}
}
