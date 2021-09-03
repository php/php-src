<?php

/** @generate-class-entries */

class XMLReader
{
    public int $attributeCount;

    public string $baseURI;

    public int $depth;

    public bool $hasAttributes;

    public bool $hasValue;

    public bool $isDefault;

    public bool $isEmptyElement;

    public string $localName;

    public string $name;

    public string $namespaceURI;

    public int $nodeType;

    public string $prefix;

    public string $value;

    public string $xmlLang;

    /** @return bool */
    public function close() {} // TODO make the return type void

    /** @tentative-return-type */
    public function getAttribute(string $name): ?string {}

    /** @tentative-return-type */
    public function getAttributeNo(int $index): ?string {}

    /** @tentative-return-type */
    public function getAttributeNs(string $name, string $namespace): ?string {}

    /** @tentative-return-type */
    public function getParserProperty(int $property): bool {}

    /** @tentative-return-type */
    public function isValid(): bool {}

    /** @tentative-return-type */
    public function lookupNamespace(string $prefix): ?string {}

    /** @tentative-return-type */
    public function moveToAttribute(string $name): bool {}

    /** @tentative-return-type */
    public function moveToAttributeNo(int $index): bool {}

    /** @tentative-return-type */
    public function moveToAttributeNs(string $name, string $namespace): bool {}

    /** @tentative-return-type */
    public function moveToElement(): bool {}

    /** @tentative-return-type */
    public function moveToFirstAttribute(): bool {}

    /** @tentative-return-type */
    public function moveToNextAttribute(): bool {}

    /** @tentative-return-type */
    public function read(): bool {}

    /** @tentative-return-type */
    public function next(?string $name = null): bool {}

    /** @return bool|XMLReader */
    public static function open(string $uri, ?string $encoding = null, int $flags = 0) {} // TODO Return type shouldn't be dependent on the call scope

    /** @tentative-return-type */
    public function readInnerXml(): string {}

    /** @tentative-return-type */
    public function readOuterXml(): string {}

    /** @tentative-return-type */
    public function readString(): string {}

    /** @tentative-return-type */
    public function setSchema(?string $filename): bool {}

    /** @tentative-return-type */
    public function setParserProperty(int $property, bool $value): bool {}

    /** @tentative-return-type */
    public function setRelaxNGSchema(?string $filename): bool {}

    /** @tentative-return-type */
    public function setRelaxNGSchemaSource(?string $source): bool {}

    /** @return bool|XMLReader */
    public static function XML(string $source, ?string $encoding = null, int $flags = 0) {} // TODO Return type shouldn't be dependent on the call scope

    /** @tentative-return-type */
    public function expand(?DOMNode $baseNode = null): DOMNode|false {}
}
