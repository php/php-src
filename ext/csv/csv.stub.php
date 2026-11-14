<?php

/** @generate-class-entries */

namespace Csv;

function array_to_row(array $fields, string $delimiter = ',', string $enclosure = '"', string $eolSequence = "\r\n"): string {}

function row_to_array(string $row, string $delimiter = ',', string $enclosure = '"', string $eolSequence = "\r\n"): array {}

function collection_to_buffer(iterable $collection, string $delimiter = ',', string $enclosure = '"', string $eolSequence = "\r\n"): string {}

function collection_to_file(string $file, iterable $collection, string $delimiter = ',', string $enclosure = '"', string $eolSequence = "\r\n"): void {}

function buffer_to_collection(string $buffer, string $delimiter = ',', string $enclosure = '"', string $eolSequence = "\r\n"): array {}

function buffer_to_collection_lax(string $buffer, string $delimiter = ',', string $enclosure = '"', string $eolSequence = "\r\n"): array {}

/**
 * @not-serializable
 * @strict-properties
 */
final class LazyLaxCollection implements \IteratorAggregate
{
    private function __construct() {}

    public function getIterator(): \InternalIterator {}

    public static function createFromBuffer(
        string $buffer,
        string $delimiter = ',',
        string $enclosure = '"',
        string $eolSequence = "\r\n"
    ): LazyLaxCollection {}

    public static function createFromFile(
        string $file,
        string $delimiter = ',',
        string $enclosure = '"',
        string $eolSequence = "\r\n"
    ): LazyLaxCollection {}
}
