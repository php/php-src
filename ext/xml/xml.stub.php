<?php

/** @generate-function-entries */

function xml_parser_create(?string $encoding = null): XmlParser {}

function xml_parser_create_ns(?string $encoding = null, string $separator = ":"): XmlParser {}

function xml_set_object(XmlParser $parser, object $object): bool {}

/**
 * @param callable $start_handler
 * @param callable $end_handler
 */
function xml_set_element_handler(XmlParser $parser, $start_handler, $end_handler): bool {}

/** @param callable $handler */
function xml_set_character_data_handler(XmlParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_processing_instruction_handler(XmlParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_default_handler(XmlParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_unparsed_entity_decl_handler(XmlParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_notation_decl_handler(XmlParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_external_entity_ref_handler(XmlParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_start_namespace_decl_handler(XmlParser $parser, $handler): bool {}

/** @param callable $handler */
function xml_set_end_namespace_decl_handler(XmlParser $parser, $handler): bool {}

function xml_parse(XmlParser $parser, string $data, bool $is_final = false): int {}

/**
 * @param array $values
 * @param array $index
 */
function xml_parse_into_struct(XmlParser $parser, string $data, &$values, &$index = null): int {}

function xml_get_error_code(XmlParser $parser): int {}

function xml_error_string(int $error_code): ?string {}

function xml_get_current_line_number(XmlParser $parser): int {}

function xml_get_current_column_number(XmlParser $parser): int {}

function xml_get_current_byte_index(XmlParser $parser): int {}

function xml_parser_free(XmlParser $parser): bool {}

/** @param string|int $value */
function xml_parser_set_option(XmlParser $parser, int $option, $value): bool {}

function xml_parser_get_option(XmlParser $parser, int $option): string|int {}

final class XMLParser
{
}
