<?php

/** @generate-class-entries */

function xml_parser_create(?string $encoding = null): XMLParser {}

function xml_parser_create_ns(?string $encoding = null, string $separator = ":"): XMLParser {}

function xml_set_object(XMLParser $parser, object $object): bool {}

/**
 * @param callable $start_handler
 * @param callable $end_handler
 */
function xml_set_element_handler(XMLParser $parser, $start_handler, $end_handler): bool {}

/** @param callable $handler */
function xml_set_character_data_handler(XMLParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_processing_instruction_handler(XMLParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_default_handler(XMLParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_unparsed_entity_decl_handler(XMLParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_notation_decl_handler(XMLParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_external_entity_ref_handler(XMLParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_start_namespace_decl_handler(XMLParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_end_namespace_decl_handler(XMLParser $parser, $handler): bool {}

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

/** @param string|int $value */
function xml_parser_set_option(XMLParser $parser, int $option, $value): bool {}

/** @refcount 1 */
function xml_parser_get_option(XMLParser $parser, int $option): string|int {}

/**
 * @strict-properties
 * @not-serializable
 */
final class XMLParser
{
}
