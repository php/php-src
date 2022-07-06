<?php

/** @generate-function-entries */

class XMLReader
{
    /** @return bool */
    public function close() {}

    /** @return string|null */
    public function getAttribute(string $name) {}

    /** @return string|null */
    public function getAttributeNo(int $index) {}

    /** @return string|null */
    public function getAttributeNs(string $name, string $namespace) {}

    /** @return bool */
    public function getParserProperty(int $property) {}

    /** @return bool */
    public function isValid() {}

    /** @return string|null */
    public function lookupNamespace(string $prefix) {}

    /** @return bool */
    public function moveToAttribute(string $name) {}

    /** @return bool */
    public function moveToAttributeNo(int $index) {}

    /** @return bool */
    public function moveToAttributeNs(string $name, string $namespace) {}

    /** @return bool */
    public function moveToElement() {}

    /** @return bool */
    public function moveToFirstAttribute() {}

    /** @return bool */
    public function moveToNextAttribute() {}

    /** @return bool */
    public function read() {}

    /** @return bool */
    public function next(?string $name = null) {}

    /** @return bool|XMLReader */
    public static function open(string $uri, ?string $encoding = null, int $flags = 0) {}

    /** @return string */
    public function readInnerXml() {}

    /** @return string */
    public function readOuterXml() {}

    /** @return string */
    public function readString() {}

    /** @return bool */
    public function setSchema(?string $filename) {}

    /** @return bool */
    public function setParserProperty(int $property, bool $value) {}

    /** @return bool */
    public function setRelaxNGSchema(?string $filename) {}

    /** @return bool */
    public function setRelaxNGSchemaSource(?string $source) {}

    /** @return bool|XMLReader */
    public static function XML(string $source, ?string $encoding = null, int $flags = 0) {}

    /** @return DOMNode|false */
    public function expand(?DOMNode $baseNode = null) {}
}
