<?php

class XMLReader
{
    /** @return bool */
    function close() {}

    /** @return string|null|false */
    function getAttribute(string $name) {}

    /** @return ?string */
    function getAttributeNo(int $index) {}

    /** @return string|null|false */
    function getAttributeNs(string $name, string $namespaceURI) {}

    /** @return bool */
    function getParserProperty(int $property) {}

    /** @return bool */
    function isValid() {}

    /** @return string|null|false */
    function lookupNamespace(string $prefix) {}

    /** @return bool */
    function moveToAttribute(string $name) {}

    /** @return bool */
    function moveToAttributeNo(int $index) {}

    /** @return bool */
    function moveToAttributeNs(string $name, string $namespaceURI) {}

    /** @return bool */
    function moveToElement() {}

    /** @return bool */
    function moveToFirstAttribute() {}

    /** @return bool */
    function moveToNextAttribute() {}

    /** @return bool */
    function read() {}

    /** @return bool */
    function next(string $localname = UNKNOWN) {}

    /** @return bool|XMLReader */
    function open(string $URI, ?string $encoding = null, int $options = 0) {}

    /** @return string */
    function readInnerXml() {}

    /** @return string */
    function readOuterXml() {}

    /** @return string */
    function readString() {}

    /** @return bool */
    function setSchema(?string $filename) {}

    /** @return bool */
    function setParserProperty(int $property, bool $value) {}

    /** @return bool */
    function setRelaxNGSchema(?string $filename) {}

    /** @return bool */
    function setRelaxNGSchemaSource(?string $source) {}

    /** @return bool|XMLReader */
    function XML(string $source, ?string $encoding = null, int $options = 0) {}

    /** @return DOMNode|bool */
    function expand(?DOMNode $basenode = null) {}
}
