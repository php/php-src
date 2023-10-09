<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue XML_ERROR_NONE
 */
const XML_ERROR_NONE = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_NO_MEMORY
 */
const XML_ERROR_NO_MEMORY = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_SYNTAX
 */
const XML_ERROR_SYNTAX = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_NO_ELEMENTS
 */
const XML_ERROR_NO_ELEMENTS = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_INVALID_TOKEN
 */
const XML_ERROR_INVALID_TOKEN = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_UNCLOSED_TOKEN
 */
const XML_ERROR_UNCLOSED_TOKEN = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_PARTIAL_CHAR
 */
const XML_ERROR_PARTIAL_CHAR = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_TAG_MISMATCH
 */
const XML_ERROR_TAG_MISMATCH = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_DUPLICATE_ATTRIBUTE
 */
const XML_ERROR_DUPLICATE_ATTRIBUTE = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_JUNK_AFTER_DOC_ELEMENT
 */
const XML_ERROR_JUNK_AFTER_DOC_ELEMENT = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_PARAM_ENTITY_REF
 */
const XML_ERROR_PARAM_ENTITY_REF = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_UNDEFINED_ENTITY
 */
const XML_ERROR_UNDEFINED_ENTITY = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_RECURSIVE_ENTITY_REF
 */
const XML_ERROR_RECURSIVE_ENTITY_REF = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_ASYNC_ENTITY
 */
const XML_ERROR_ASYNC_ENTITY = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_BAD_CHAR_REF
 */
const XML_ERROR_BAD_CHAR_REF = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_BINARY_ENTITY_REF
 */
const XML_ERROR_BINARY_ENTITY_REF = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF
 */
const XML_ERROR_ATTRIBUTE_EXTERNAL_ENTITY_REF = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_MISPLACED_XML_PI
 */
const XML_ERROR_MISPLACED_XML_PI = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_UNKNOWN_ENCODING
 */
const XML_ERROR_UNKNOWN_ENCODING = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_INCORRECT_ENCODING
 */
const XML_ERROR_INCORRECT_ENCODING = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_UNCLOSED_CDATA_SECTION
 */
const XML_ERROR_UNCLOSED_CDATA_SECTION = UNKNOWN;
/**
 * @var int
 * @cvalue XML_ERROR_EXTERNAL_ENTITY_HANDLING
 */
const XML_ERROR_EXTERNAL_ENTITY_HANDLING = UNKNOWN;

/**
 * @var int
 * @cvalue PHP_XML_OPTION_CASE_FOLDING
 */
const XML_OPTION_CASE_FOLDING = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_XML_OPTION_TARGET_ENCODING
 */
const XML_OPTION_TARGET_ENCODING = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_XML_OPTION_SKIP_TAGSTART
 */
const XML_OPTION_SKIP_TAGSTART = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_XML_OPTION_SKIP_WHITE
 */
const XML_OPTION_SKIP_WHITE = UNKNOWN;

/**
 * @var string
 * @cvalue PHP_XML_SAX_IMPL
 */
const XML_SAX_IMPL = UNKNOWN;

function xml_parser_create(?string $encoding = null): XMLParser {}

function xml_parser_create_ns(?string $encoding = null, string $separator = ":"): XMLParser {}

function xml_set_object(XMLParser $parser, object $object): true {}

/**
 * @param callable $start_handler
 * @param callable $end_handler
 */
function xml_set_element_handler(XMLParser $parser, $start_handler, $end_handler): true {}

/** @param callable $handler */
function xml_set_character_data_handler(XMLParser $parser, $handler): true {}

/** @param callable $handler */
function xml_set_processing_instruction_handler(XMLParser $parser, $handler): true {}

/** @param callable $handler */
function xml_set_default_handler(XMLParser $parser, $handler): true {}

/** @param callable $handler */
function xml_set_unparsed_entity_decl_handler(XMLParser $parser, $handler): true {}

/** @param callable $handler */
function xml_set_notation_decl_handler(XMLParser $parser, $handler): true {}

/** @param callable $handler */
function xml_set_external_entity_ref_handler(XMLParser $parser, $handler): true {}

/** @param callable $handler */
function xml_set_start_namespace_decl_handler(XMLParser $parser, $handler): true {}

/** @param callable $handler */
function xml_set_end_namespace_decl_handler(XMLParser $parser, $handler): true {}

function xml_parse(XMLParser $parser, string $data, bool $is_final = false): int {}

/**
 * @param array $values
 * @param array $index
 */
function xml_parse_into_struct(XMLParser $parser, string $data, &$values, &$index = null): int|false {}

function xml_get_error_code(XMLParser $parser): int {}

/** @refcount 1 */
function xml_error_string(int $error_code): ?string {}

function xml_get_current_line_number(XMLParser $parser): int {}

function xml_get_current_column_number(XMLParser $parser): int {}

function xml_get_current_byte_index(XMLParser $parser): int {}

function xml_parser_free(XMLParser $parser): bool {}

/** @param string|int|bool $value */
function xml_parser_set_option(XMLParser $parser, int $option, $value): bool {}

/** @refcount 1 */
function xml_parser_get_option(XMLParser $parser, int $option): string|int|bool {}

/**
 * @strict-properties
 * @not-serializable
 */
final class XMLParser
{
}
