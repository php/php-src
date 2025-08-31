<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue PHP_JSON_HEX_TAG
 */
const JSON_HEX_TAG = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_HEX_AMP
 */
const JSON_HEX_AMP = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_HEX_APOS
 */
const JSON_HEX_APOS = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_HEX_QUOT
 */
const JSON_HEX_QUOT = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_FORCE_OBJECT
 */
const JSON_FORCE_OBJECT = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_NUMERIC_CHECK
 */
const JSON_NUMERIC_CHECK = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_UNESCAPED_SLASHES
 */
const JSON_UNESCAPED_SLASHES = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_PRETTY_PRINT
 */
const JSON_PRETTY_PRINT = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_UNESCAPED_UNICODE
 */
const JSON_UNESCAPED_UNICODE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_PARTIAL_OUTPUT_ON_ERROR
 */
const JSON_PARTIAL_OUTPUT_ON_ERROR = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_PRESERVE_ZERO_FRACTION
 */
const JSON_PRESERVE_ZERO_FRACTION = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_UNESCAPED_LINE_TERMINATORS
 */
const JSON_UNESCAPED_LINE_TERMINATORS = UNKNOWN;

/**
 * @var int
 * @cvalue PHP_JSON_OBJECT_AS_ARRAY
 */
const JSON_OBJECT_AS_ARRAY = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_BIGINT_AS_STRING
 */
const JSON_BIGINT_AS_STRING = UNKNOWN;

/**
 * @var int
 * @cvalue PHP_JSON_INVALID_UTF8_IGNORE
 */
const JSON_INVALID_UTF8_IGNORE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_INVALID_UTF8_SUBSTITUTE
 */
const JSON_INVALID_UTF8_SUBSTITUTE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_THROW_ON_ERROR
 */
const JSON_THROW_ON_ERROR = UNKNOWN;

/**
 * @var int
 * @cvalue PHP_JSON_ERROR_NONE
 */
const JSON_ERROR_NONE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_DEPTH
 */
const JSON_ERROR_DEPTH = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_STATE_MISMATCH
 */
const JSON_ERROR_STATE_MISMATCH = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_CTRL_CHAR
 */
const JSON_ERROR_CTRL_CHAR = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_SYNTAX
 */
const JSON_ERROR_SYNTAX = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_UTF8
 */
const JSON_ERROR_UTF8 = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_RECURSION
 */
const JSON_ERROR_RECURSION = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_INF_OR_NAN
 */
const JSON_ERROR_INF_OR_NAN = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_UNSUPPORTED_TYPE
 */
const JSON_ERROR_UNSUPPORTED_TYPE = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_INVALID_PROPERTY_NAME
 */
const JSON_ERROR_INVALID_PROPERTY_NAME = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_UTF16
 */
const JSON_ERROR_UTF16 = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_JSON_ERROR_NON_BACKED_ENUM
 */
const JSON_ERROR_NON_BACKED_ENUM = UNKNOWN;

/** @refcount 1 */
function json_encode(mixed $value, int $flags = 0, int $depth = 512): string|false {}

function json_decode(string $json, ?bool $associative = null, int $depth = 512, int $flags = 0): mixed {}

function json_validate(string $json, int $depth = 512, int $flags = 0): bool {}

function json_last_error(): int {}

/** @refcount 1 */
function json_last_error_msg(): string {}

interface JsonSerializable
{
    /** @tentative-return-type */
    public function jsonSerialize(): mixed;
}

class JsonException extends Exception
{
}
