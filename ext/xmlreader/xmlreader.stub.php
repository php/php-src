<?php

class XMLReader
{
    function close(): bool {}

    /** @return string|null|false */
    function getAttribute(string $name) {}

    function getAttributeNo(int $index): ?string {}

    /** @return string|null|false */
    function getAttributeNs(string $name, string $namespaceURI) {}

    function getParserProperty(int $property): bool {}

    function isValid(): bool {}

    /** @return string|null|false */
    function lookupNamespace(string $prefix) {}

    function moveToAttribute(string $name): bool {}

    function moveToAttributeNo(int $index): bool {}

    function moveToAttributeNs(string $name, string $namespaceURI): bool {}

    function moveToElement(): bool {}

    function moveToFirstAttribute(): bool {}

    function moveToNextAttribute(): bool {}

    function read(): bool {}

    function next(string $localname = UNKNOWN): bool {}

    function open(string $URI, ?string $encoding = null, int $options = 0): bool {}

    function readInnerXml(): string {}

    function readOuterXml(): string {}

    function readString(): string {}

    function setSchema(?string $filename) {}

    function setParserProperty(int $property, bool $value): bool {}

    function setRelaxNGSchema(?string $filename): bool {}

    function setRelaxNGSchemaSource(?string $source): bool {}

    function XML(string $source, ?string $encoding = null, int $options = 0): bool {}

    /** @return DOMNode|bool */
    function expand(?DOMNode $basenode = null) {}
}
