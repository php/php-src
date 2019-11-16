<?php

function simplexml_load_file(string $filename, ?string $class_name = SimpleXMLElement::class, int $options = 0, string $ns = '', bool $is_prefix = false): SimpleXMLElement|false {}

function simplexml_load_string(string $data, ?string $class_name = SimpleXMLElement::class, int $options = 0, string $ns = '', bool $is_prefix = false): SimpleXMLElement|false {}

function simplexml_import_dom(DOMNode $node, ?string $class_name = SimpleXMLElement::class): ?SimpleXMLElement {}

class SimpleXMLElement
{
    /** @return array|false */
    public function xpath(string $path) {}

    /** @return bool */
    public function registerXPathNamespace(string $prefix, string $ns) {}

    /** @return string|bool */
    public function asXML(string $filename = UNKNOWN) {}

    /** @return string|bool */
    public function saveXML(string $filename = UNKNOWN) {}

    /** @return array */
    public function getNamespaces(bool $recursive = false) {}

    /** @return array|false */
    public function getDocNamespaces(bool $recursive = false, bool $from_root = true) {}

    /** @return SimpleXMLIterator */
    public function children(?string $ns = null, bool $is_prefix = false) {}

    /** @return SimpleXMLIterator */
    public function attributes(?string $ns = null, bool $is_prefix = false) {}

    public function __construct(string $data, int $options = 0, bool $data_is_url = false, string $ns = '', bool $is_prefix = false) {}

    /** @return SimpleXMLElement */
    public function addChild(string $name, ?string $value = null, ?string $ns = null) {}

    /** @return SimpleXMLElement */
    public function addAttribute(string $name, ?string $value = null, ?string $ns = null) {}

    /** @return string */
    public function getName() {}

    /** @return string */
    public function __toString() {}

    /** @return int */
    public function count() {}
}
