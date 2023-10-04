<?php

/** @generate-class-entries */

/** @not-serializable */
class UConverter
{
    /* enum UConverterCallbackReason */

    /**
     * @cvalue UCNV_UNASSIGNED
     */
    public const int REASON_UNASSIGNED = UNKNOWN;
    /**
     * @cvalue UCNV_ILLEGAL
     */
    public const int REASON_ILLEGAL = UNKNOWN;
    /**
     * @cvalue UCNV_IRREGULAR
     */
    public const int REASON_IRREGULAR = UNKNOWN;
    /**
     * @cvalue UCNV_RESET
     */
    public const int REASON_RESET = UNKNOWN;
    /**
     * @cvalue UCNV_CLOSE
     */
    public const int REASON_CLOSE = UNKNOWN;
    /**
     * @cvalue UCNV_CLONE
     */
    public const int REASON_CLONE = UNKNOWN;

    /* enum UConverterType */

    /**
     * @cvalue UCNV_UNSUPPORTED_CONVERTER
     */
    public const int UNSUPPORTED_CONVERTER = UNKNOWN;
    /**
     * @cvalue UCNV_SBCS
     */
    public const int SBCS = UNKNOWN;
    /**
     * @cvalue UCNV_DBCS
     */
    public const int DBCS = UNKNOWN;
    /**
     * @cvalue UCNV_MBCS
     */
    public const int MBCS = UNKNOWN;
    /**
     * @cvalue UCNV_LATIN_1
     */
    public const int LATIN_1 = UNKNOWN;
    /**
     * @cvalue UCNV_UTF8
     */
    public const int UTF8 = UNKNOWN;
    /**
     * @cvalue UCNV_UTF16_BigEndian
     */
    public const int UTF16_BigEndian = UNKNOWN;
    /**
     * @cvalue UCNV_UTF16_LittleEndian
     */
    public const int UTF16_LittleEndian = UNKNOWN;
    /**
     * @cvalue UCNV_UTF32_BigEndian
     */
    public const int UTF32_BigEndian = UNKNOWN;
    /**
     * @cvalue UCNV_UTF32_LittleEndian
     */
    public const int UTF32_LittleEndian = UNKNOWN;
    /**
     * @cvalue UCNV_EBCDIC_STATEFUL
     */
    public const int EBCDIC_STATEFUL = UNKNOWN;
    /**
     * @cvalue UCNV_ISO_2022
     */
    public const int ISO_2022 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_1
     */
    public const int LMBCS_1 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_2
     */
    public const int LMBCS_2 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_3
     */
    public const int LMBCS_3 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_4
     */
    public const int LMBCS_4 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_5
     */
    public const int LMBCS_5 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_6
     */
    public const int LMBCS_6 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_8
     */
    public const int LMBCS_8 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_11
     */
    public const int LMBCS_11 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_16
     */
    public const int LMBCS_16 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_17
     */
    public const int LMBCS_17 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_18
     */
    public const int LMBCS_18 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_19
     */
    public const int LMBCS_19 = UNKNOWN;
    /**
     * @cvalue UCNV_LMBCS_LAST
     */
    public const int LMBCS_LAST = UNKNOWN;
    /**
     * @cvalue UCNV_HZ
     */
    public const int HZ = UNKNOWN;
    /**
     * @cvalue UCNV_SCSU
     */
    public const int SCSU = UNKNOWN;
    /**
     * @cvalue UCNV_ISCII
     */
    public const int ISCII = UNKNOWN;
    /**
     * @cvalue UCNV_US_ASCII
     */
    public const int US_ASCII = UNKNOWN;
    /**
     * @cvalue UCNV_UTF7
     */
    public const int UTF7 = UNKNOWN;
    /**
     * @cvalue UCNV_BOCU1
     */
    public const int BOCU1 = UNKNOWN;
    /**
     * @cvalue UCNV_UTF16
     */
    public const int UTF16 = UNKNOWN;
    /**
     * @cvalue UCNV_UTF32
     */
    public const int UTF32 = UNKNOWN;
    /**
     * @cvalue UCNV_CESU8
     */
    public const int CESU8 = UNKNOWN;
    /**
     * @cvalue UCNV_IMAP_MAILBOX
     */
    public const int IMAP_MAILBOX = UNKNOWN;

    public function __construct(?string $destination_encoding = null, ?string $source_encoding = null) {}

    /** @tentative-return-type */
    public function convert(string $str, bool $reverse = false): string|false {}

    /**
     * @param int $error
     * @tentative-return-type
     */
    public function fromUCallback(int $reason, array $source, int $codePoint, &$error): string|int|array|null {}

    /**
     * @return array<int, string>|false|null
     * @tentative-return-type
     */
    public static function getAliases(string $name): array|false|null {}

    /**
     * @return array<int, string>
     * @tentative-return-type
     */
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
