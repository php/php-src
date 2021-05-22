<?php

/** @generate-function-entries */

function simplexml_load_file(string $filename, ?string $class_name = SimpleXMLElement::class, int $options = 0, string $namespace_or_prefix = "", bool $is_prefix = false): SimpleXMLElement|false {}

function simplexml_load_string(string $data, ?string $class_name = SimpleXMLElement::class, int $options = 0, string $namespace_or_prefix = "", bool $is_prefix = false): SimpleXMLElement|false {}

function simplexml_import_dom(SimpleXMLElement|DOMNode $node, ?string $class_name = SimpleXMLElement::class): ?SimpleXMLElement {}

class SimpleXMLElement implements Stringable, Countable, RecursiveIterator
{
    /** @return array|null|false */
    public function xpath(string $expression) {}

    /** @return bool */
    public function registerXPathNamespace(string $prefix, string $namespace) {}

    /** @return string|bool */
    public function asXML(?string $filename = null) {}

    /**
     * @return string|bool
     * @alias SimpleXMLElement::asXML
     */
    public function saveXML(?string $filename = null) {}

    /** @return array */
    public function getNamespaces(bool $recursive = false) {}

    /** @return array|false */
    public function getDocNamespaces(bool $recursive = false, bool $fromRoot = true) {}

    /** @return SimpleXMLElement|null */
    public function children(?string $namespaceOrPrefix = null, bool $isPrefix = false) {}

    /** @return SimpleXMLElement|null */
    public function attributes(?string $namespaceOrPrefix = null, bool $isPrefix = false) {}

    public function __construct(string $data, int $options = 0, bool $dataIsURL = false, string $namespaceOrPrefix = "", bool $isPrefix = false) {}

    /** @return SimpleXMLElement|null */
    public function addChild(string $qualifiedName, ?string $value = null, ?string $namespace = null) {}

    /** @return void */
    public function addAttribute(string $qualifiedName, string $value, ?string $namespace = null) {}

    /** @return string */
    public function getName() {}

    public function __toString(): string {}

    /** @return int */
    public function count() {}

    /** @return void */
    public function rewind() {}

    /** @return bool */
    public function valid() {}

    /** @return SimpleXMLElement|null */
    public function current() {}

    /** @return string|false */
    public function key() {}

    /** @return void */
    public function next() {}

    /** @return bool */
    public function hasChildren() {}

    /** @return SimpleXMLElement|null */
    public function getChildren() {}
}

class SimpleXMLIterator extends SimpleXMLElement
{
}
