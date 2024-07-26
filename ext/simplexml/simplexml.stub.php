<?php

/** @generate-class-entries */

function simplexml_load_file(string $filename, ?string $class_name = SimpleXMLElement::class, int $options = 0, string $namespace_or_prefix = "", bool $is_prefix = false): SimpleXMLElement|false {}

function simplexml_load_string(string $data, ?string $class_name = SimpleXMLElement::class, int $options = 0, string $namespace_or_prefix = "", bool $is_prefix = false): SimpleXMLElement|false {}

function simplexml_import_dom(object $node, ?string $class_name = SimpleXMLElement::class): ?SimpleXMLElement {}

/** @not-serializable */
class SimpleXMLElement implements Stringable, Countable, RecursiveIterator, DimensionFetchable, DimensionWritable, FetchAppendable, DimensionUnsetable
{
    /** @tentative-return-type */
    public function xpath(string $expression): array|null|false {}

    /** @tentative-return-type */
    public function registerXPathNamespace(string $prefix, string $namespace): bool {}

    /** @tentative-return-type */
    public function asXML(?string $filename = null): string|bool {}

    /**
     * @tentative-return-type
     * @alias SimpleXMLElement::asXML
     */
    public function saveXML(?string $filename = null): string|bool {}

    /** @tentative-return-type */
    public function getNamespaces(bool $recursive = false): array {}

    /** @tentative-return-type */
    public function getDocNamespaces(bool $recursive = false, bool $fromRoot = true): array|false {}

    /** @tentative-return-type */
    public function children(?string $namespaceOrPrefix = null, bool $isPrefix = false): ?SimpleXMLElement {}

    /** @tentative-return-type */
    public function attributes(?string $namespaceOrPrefix = null, bool $isPrefix = false): ?SimpleXMLElement {}

    public function __construct(string $data, int $options = 0, bool $dataIsURL = false, string $namespaceOrPrefix = "", bool $isPrefix = false) {}

    /** @tentative-return-type */
    public function addChild(string $qualifiedName, ?string $value = null, ?string $namespace = null): ?SimpleXMLElement {}

    /** @tentative-return-type */
    public function addAttribute(string $qualifiedName, string $value, ?string $namespace = null): void {}

    /** @tentative-return-type */
    public function getName(): string {}

    public function __toString(): string {}

    /** @tentative-return-type */
    public function count(): int {}

    /** @tentative-return-type */
    public function rewind(): void {}

    /** @tentative-return-type */
    public function valid(): bool {}

    /** @tentative-return-type */
    public function current(): SimpleXMLElement {}

    /** @tentative-return-type */
    public function key(): string {}

    /** @tentative-return-type */
    public function next(): void {}

    /** @tentative-return-type */
    public function hasChildren(): bool {}

    /** @tentative-return-type */
    public function getChildren(): ?SimpleXMLElement {}

    public function offsetGet(mixed $offset): mixed {}

    public function &offsetFetch(mixed $offset): mixed {}

    public function offsetExists(mixed $offset): bool {}

    public function offsetSet(mixed $offset, mixed $value): void {}

    public function append(mixed $value): void{}

    public function &fetchAppend(): mixed {}

    public function offsetUnset(mixed $offset): void {}
}

class SimpleXMLIterator extends SimpleXMLElement
{
}
