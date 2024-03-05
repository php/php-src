<?php

/** @generate-class-entries */

class XMLReader
{
    /* Constants for NodeType - cannot define common types to share with dom as there are differences in these types */

    /**
     * @cvalue XML_READER_TYPE_NONE
     */
    public const int NONE = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_ELEMENT
     */
    public const int ELEMENT = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_ATTRIBUTE
     */
    public const int ATTRIBUTE = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_TEXT
     */
    public const int TEXT = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_CDATA
     */
    public const int CDATA = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_ENTITY_REFERENCE
     */
    public const int ENTITY_REF = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_ENTITY
     */
    public const int ENTITY = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_PROCESSING_INSTRUCTION
     */
    public const int PI = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_COMMENT
     */
    public const int COMMENT = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_DOCUMENT
     */
    public const int DOC = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_DOCUMENT_TYPE
     */
    public const int DOC_TYPE = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_DOCUMENT_FRAGMENT
     */
    public const int DOC_FRAGMENT = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_NOTATION
     */
    public const int NOTATION = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_WHITESPACE
     */
    public const int WHITESPACE = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_SIGNIFICANT_WHITESPACE
     */
    public const int SIGNIFICANT_WHITESPACE = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_END_ELEMENT
     */
    public const int END_ELEMENT = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_END_ENTITY
     */
    public const int END_ENTITY = UNKNOWN;
    /**
     * @cvalue XML_READER_TYPE_XML_DECLARATION
     */
    public const int XML_DECLARATION = UNKNOWN;

    /* Constants for Parser options */

    /**
     * @cvalue XML_PARSER_LOADDTD
     */
    public const int LOADDTD = UNKNOWN;
    /**
     * @cvalue XML_PARSER_DEFAULTATTRS
     */
    public const int DEFAULTATTRS = UNKNOWN;
    /**
     * @cvalue XML_PARSER_VALIDATE
     */
    public const int VALIDATE = UNKNOWN;
    /**
     * @cvalue XML_PARSER_SUBST_ENTITIES
     */
    public const int SUBST_ENTITIES = UNKNOWN;


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

    /** @return true */
    public function close() {} // TODO make return type void

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
