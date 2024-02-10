<?php

/** @generate-class-entries */

namespace
{
    /**
     * @var int
     * @cvalue XML_ELEMENT_NODE
     */
    const XML_ELEMENT_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_NODE
     */
    const XML_ATTRIBUTE_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_TEXT_NODE
     */
    const XML_TEXT_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_CDATA_SECTION_NODE
     */
    const XML_CDATA_SECTION_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ENTITY_REF_NODE
     */
    const XML_ENTITY_REF_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ENTITY_NODE
     */
    const XML_ENTITY_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_PI_NODE
     */
    const XML_PI_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_COMMENT_NODE
     */
    const XML_COMMENT_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_DOCUMENT_NODE
     */
    const XML_DOCUMENT_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_DOCUMENT_TYPE_NODE
     */
    const XML_DOCUMENT_TYPE_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_DOCUMENT_FRAG_NODE
     */
    const XML_DOCUMENT_FRAG_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_NOTATION_NODE
     */
    const XML_NOTATION_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_HTML_DOCUMENT_NODE
     */
    const XML_HTML_DOCUMENT_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_DTD_NODE
     */
    const XML_DTD_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ELEMENT_DECL
     */
    const XML_ELEMENT_DECL_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_DECL
     */
    const XML_ATTRIBUTE_DECL_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ENTITY_DECL
     */
    const XML_ENTITY_DECL_NODE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_NAMESPACE_DECL
     */
    const XML_NAMESPACE_DECL_NODE = UNKNOWN;
#ifdef XML_GLOBAL_NAMESPACE
    /**
     * @var int
     * @cvalue XML_GLOBAL_NAMESPACE
     */
    const XML_GLOBAL_NAMESPACE = UNKNOWN;
#endif

    /**
     * @var int
     * @cvalue XML_LOCAL_NAMESPACE
     */
    const XML_LOCAL_NAMESPACE = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_CDATA
     */
    const XML_ATTRIBUTE_CDATA = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_ID
     */
    const XML_ATTRIBUTE_ID = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_IDREF
     */
    const XML_ATTRIBUTE_IDREF = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_IDREFS
     */
    const XML_ATTRIBUTE_IDREFS = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_ENTITIES
     */
    const XML_ATTRIBUTE_ENTITY = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_NMTOKEN
     */
    const XML_ATTRIBUTE_NMTOKEN = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_NMTOKENS
     */
    const XML_ATTRIBUTE_NMTOKENS = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_ENUMERATION
     */
    const XML_ATTRIBUTE_ENUMERATION = UNKNOWN;
    /**
     * @var int
     * @cvalue XML_ATTRIBUTE_NOTATION
     */
    const XML_ATTRIBUTE_NOTATION = UNKNOWN;

    /**
     * @var int
     * @cvalue PHP_ERR
     */
    const DOM_PHP_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INDEX_SIZE_ERR
     */
    const DOM_INDEX_SIZE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue DOMSTRING_SIZE_ERR
     */
    const DOMSTRING_SIZE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue HIERARCHY_REQUEST_ERR
     */
    const DOM_HIERARCHY_REQUEST_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue WRONG_DOCUMENT_ERR
     */
    const DOM_WRONG_DOCUMENT_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INVALID_CHARACTER_ERR
     */
    const DOM_INVALID_CHARACTER_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NO_DATA_ALLOWED_ERR
     */
    const DOM_NO_DATA_ALLOWED_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NO_MODIFICATION_ALLOWED_ERR
     */
    const DOM_NO_MODIFICATION_ALLOWED_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NOT_FOUND_ERR
     */
    const DOM_NOT_FOUND_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NOT_SUPPORTED_ERR
     */
    const DOM_NOT_SUPPORTED_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INUSE_ATTRIBUTE_ERR
     */
    const DOM_INUSE_ATTRIBUTE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INVALID_STATE_ERR
     */
    const DOM_INVALID_STATE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue SYNTAX_ERR
     */
    const DOM_SYNTAX_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INVALID_MODIFICATION_ERR
     */
    const DOM_INVALID_MODIFICATION_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NAMESPACE_ERR
     */
    const DOM_NAMESPACE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INVALID_ACCESS_ERR
     */
    const DOM_INVALID_ACCESS_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue VALIDATION_ERR
     */
    const DOM_VALIDATION_ERR = UNKNOWN;

    class DOMDocumentType extends DOMNode
    {
        /** @readonly */
        public string $name;

        /** @readonly */
        public DOMNamedNodeMap $entities;

        /** @readonly */
        public DOMNamedNodeMap $notations;

        /** @readonly */
        public string $publicId;

        /** @readonly */
        public string $systemId;

        /** @readonly */
        public ?string $internalSubset;
    }

    class DOMCdataSection extends DOMText
    {
        public function __construct(string $data) {}
    }

    class DOMComment extends DOMCharacterData
    {
        public function __construct(string $data = "") {}
    }

    interface DOMParentNode
    {
        /** @param DOMNode|string $nodes */
        public function append(...$nodes): void;

        /** @param DOMNode|string $nodes */
        public function prepend(...$nodes): void;

        /** @param DOMNode|string $nodes */
        public function replaceChildren(...$nodes): void;
    }

    interface DOMChildNode
    {
        public function remove(): void;

        /** @param DOMNode|string $nodes */
        public function before(... $nodes): void;

        /** @param DOMNode|string $nodes */
        public function after(...$nodes): void;

        /** @param DOMNode|string $nodes */
        public function replaceWith(...$nodes): void;
    }

    class DOMNode
    {
        public const int DOCUMENT_POSITION_DISCONNECTED = 0x01;
        public const int DOCUMENT_POSITION_PRECEDING = 0x02;
        public const int DOCUMENT_POSITION_FOLLOWING = 0x04;
        public const int DOCUMENT_POSITION_CONTAINS = 0x08;
        public const int DOCUMENT_POSITION_CONTAINED_BY = 0x10;
        public const int DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC = 0x20;

        /** @readonly */
        public string $nodeName;

        public ?string $nodeValue;

        /** @readonly */
        public int $nodeType;

        /** @readonly */
        public ?DOMNode $parentNode;

        /** @readonly */
        public ?DOMElement $parentElement;

        /** @readonly */
        public DOMNodeList $childNodes;

        /** @readonly */
        public ?DOMNode $firstChild;

        /** @readonly */
        public ?DOMNode $lastChild;

        /** @readonly */
        public ?DOMNode $previousSibling;

        /** @readonly */
        public ?DOMNode $nextSibling;

        /** @readonly */
        public ?DOMNamedNodeMap $attributes;

        /** @readonly */
        public bool $isConnected;

        /** @readonly */
        public ?DOMDocument $ownerDocument;

        /** @readonly */
        public ?string $namespaceURI;

        public string $prefix;

        /** @readonly */
        public ?string $localName;

        /** @readonly */
        public ?string $baseURI;

        public string $textContent;

        /** @return DOMNode|false */
        public function appendChild(DOMNode $node) {}

        /** @tentative-return-type */
        public function C14N(bool $exclusive = false, bool $withComments = false, ?array $xpath = null, ?array $nsPrefixes = null): string|false {}

        /** @tentative-return-type */
        public function C14NFile(string $uri, bool $exclusive = false, bool $withComments = false, ?array $xpath = null, ?array $nsPrefixes = null): int|false {}

        /** @return DOMNode|false */
        public function cloneNode(bool $deep = false) {}

        /** @tentative-return-type */
        public function getLineNo(): int {}

        /** @tentative-return-type */
        public function getNodePath(): ?string {}

        /** @tentative-return-type */
        public function hasAttributes(): bool {}

        /** @tentative-return-type */
        public function hasChildNodes(): bool {}

        /** @return DOMNode|false */
        public function insertBefore(DOMNode $node, ?DOMNode $child = null) {}

        /** @tentative-return-type */
        public function isDefaultNamespace(string $namespace): bool {}

        /** @tentative-return-type */
        public function isSameNode(DOMNode $otherNode): bool {}

        public function isEqualNode(?DOMNode $otherNode): bool {}

        /** @tentative-return-type */
        public function isSupported(string $feature, string $version): bool {}

        /** @tentative-return-type */
        public function lookupNamespaceURI(?string $prefix): ?string {}

        /** @tentative-return-type */
        public function lookupPrefix(string $namespace): ?string {}

        /** @tentative-return-type */
        public function normalize(): void {}

        /** @return DOMNode|false */
        public function removeChild(DOMNode $child) {}

        /** @return DOMNode|false */
        public function replaceChild(DOMNode $node, DOMNode $child) {}

        public function contains(DOMNode|DOMNameSpaceNode|null $other): bool {}

        public function getRootNode(?array $options = null): DOMNode {}

        public function compareDocumentPosition(DOMNode $other): int {}

        public function __sleep(): array {}
    
        public function __wakeup(): void {}
    }

    class DOMNameSpaceNode
    {
        /** @readonly */
        public string $nodeName;

        /** @readonly */
        public ?string $nodeValue;

        /** @readonly */
        public int $nodeType;

        /** @readonly */
        public string $prefix;

        /** @readonly */
        public ?string $localName;

        /** @readonly */
        public ?string $namespaceURI;

        /** @readonly */
        public bool $isConnected;

        /** @readonly */
        public ?DOMDocument $ownerDocument;

        /** @readonly */
        public ?DOMNode $parentNode;

        /** @readonly */
        public ?DOMElement $parentElement;

        /** @implementation-alias DOMNode::__sleep */
        public function __sleep(): array {}
    
        /** @implementation-alias DOMNode::__wakeup */
        public function __wakeup(): void {}
    }

    class DOMImplementation
    {
        /** @tentative-return-type */
        public function getFeature(string $feature, string $version): never {}

        /** @tentative-return-type */
        public function hasFeature(string $feature, string $version): bool {}

        /** @return DOMDocumentType|false */
        public function createDocumentType(string $qualifiedName, string $publicId = "", string $systemId = "") {}

        /** @tentative-return-type */
        public function createDocument(?string $namespace = null, string $qualifiedName = "", ?DOMDocumentType $doctype = null): DOMDocument {}
    }

    class DOMDocumentFragment extends DOMNode implements DOMParentNode
    {
        /** @readonly */
        public ?DOMElement $firstElementChild;

        /** @readonly */
        public ?DOMElement $lastElementChild;

        /** @readonly */
        public int $childElementCount;

        public function __construct() {}

        /** @tentative-return-type */
        public function appendXML(string $data): bool {}

        /**
         * @param DOMNode|string $nodes
         * @implementation-alias DOMElement::append
         */
        public function append(...$nodes): void {}

        /**
         * @param DOMNode|string $nodes
         * @implementation-alias DOMElement::prepend
         */
        public function prepend(...$nodes): void {}

        /**
         * @param DOMNode|string $nodes
         * @implementation-alias DOMDocument::replaceChildren
         */
        public function replaceChildren(...$nodes): void {}
    }

    class DOMNodeList implements IteratorAggregate, Countable
    {
        /** @readonly */
        public int $length;

        /** @tentative-return-type */
        public function count(): int {}

        public function getIterator(): Iterator {}

        /** @return DOMElement|DOMNode|DOMNameSpaceNode|null */
        public function item(int $index) {}
    }

    class DOMCharacterData extends DOMNode implements DOMChildNode
    {
        public string $data;

        /** @readonly */
        public int $length;

        /** @readonly */
        public ?DOMElement $previousElementSibling;

        /** @readonly */
        public ?DOMElement $nextElementSibling;

        /** @tentative-return-type */
        public function appendData(string $data): true {}

        /** @return string|false */
        public function substringData(int $offset, int $count) {}

        /** @tentative-return-type */
        public function insertData(int $offset, string $data): bool {}

        /** @tentative-return-type */
        public function deleteData(int $offset, int $count): bool {}

        /** @tentative-return-type */
        public function replaceData(int $offset, int $count, string $data): bool {}

        /**
         * @param DOMNode|string $nodes
         * @implementation-alias DOMElement::replaceWith
         */
        public function replaceWith(...$nodes): void {}

        /** @implementation-alias DOMElement::remove */
        public function remove(): void {}

        /**
         * @param DOMNode|string $nodes
         * @implementation-alias DOMElement::before
         */
        public function before(... $nodes): void {}

        /**
         * @param DOMNode|string $nodes
         * @implementation-alias DOMElement::after
         */
        public function after(...$nodes): void {}
    }

    class DOMAttr extends DOMNode
    {
        /** @readonly */
        public string $name;

        /** @readonly */
        public bool $specified = true;

        public string $value;

        /** @readonly */
        public ?DOMElement $ownerElement;

        /** @readonly */
        public mixed $schemaTypeInfo = null;

        public function __construct(string $name, string $value = "") {}

        /** @tentative-return-type */
        public function isId(): bool {}
    }

    class DOMElement extends DOMNode implements \DOMParentNode, \DOMChildNode
    {
        /** @readonly */
        public string $tagName;

        public string $className;

        public string $id;

        /** @readonly */
        public mixed $schemaTypeInfo = null;

        /** @readonly */
        public ?DOMElement $firstElementChild;

        /** @readonly */
        public ?DOMElement $lastElementChild;

        /** @readonly */
        public int $childElementCount;

        /** @readonly */
        public ?DOMElement $previousElementSibling;

        /** @readonly */
        public ?DOMElement $nextElementSibling;

        public function __construct(string $qualifiedName, ?string $value = null, string $namespace = "") {}

        /** @tentative-return-type */
        public function getAttribute(string $qualifiedName): string {}

        public function getAttributeNames(): array {}

        /** @tentative-return-type */
        public function getAttributeNS(?string $namespace, string $localName): string {}

        /** @return DOMAttr|DOMNameSpaceNode|false */
        public function getAttributeNode(string $qualifiedName) {}

        /** @return DOMAttr|DOMNameSpaceNode|null */
        public function getAttributeNodeNS(?string $namespace, string $localName) {}

        /** @tentative-return-type */
        public function getElementsByTagName(string $qualifiedName): DOMNodeList {}

        /** @tentative-return-type */
        public function getElementsByTagNameNS(?string $namespace, string $localName): DOMNodeList {}

        /** @tentative-return-type */
        public function hasAttribute(string $qualifiedName): bool {}

        /** @tentative-return-type */
        public function hasAttributeNS(?string $namespace, string $localName): bool {}

        /** @tentative-return-type */
        public function removeAttribute(string $qualifiedName): bool {}

        /** @tentative-return-type */
        public function removeAttributeNS(?string $namespace, string $localName): void {}

        /** @return DOMAttr|false */
        public function removeAttributeNode(DOMAttr $attr) {}

        /** @return DOMAttr|bool */
        public function setAttribute(string $qualifiedName, string $value) {}

        /** @tentative-return-type */
        public function setAttributeNS(?string $namespace, string $qualifiedName, string $value): void {}

        /** @return DOMAttr|null|false */
        public function setAttributeNode(DOMAttr $attr) {}

        /** @return DOMAttr|null|false */
        public function setAttributeNodeNS(DOMAttr $attr) {}

        /** @tentative-return-type */
        public function setIdAttribute(string $qualifiedName, bool $isId): void {}

        /** @tentative-return-type */
        public function setIdAttributeNS(string $namespace, string $qualifiedName, bool $isId): void {}

        /** @tentative-return-type */
        public function setIdAttributeNode(DOMAttr $attr, bool $isId): void {}

        public function toggleAttribute(string $qualifiedName, ?bool $force = null): bool {}

        public function remove(): void {}

        /** @param DOMNode|string $nodes */
        public function before(... $nodes): void {}

        /** @param DOMNode|string $nodes */
        public function after(...$nodes): void {}

        /** @param DOMNode|string $nodes */
        public function replaceWith(...$nodes): void {}

        /** @param DOMNode|string $nodes */
        public function append(...$nodes): void {}

        /** @param DOMNode|string $nodes */
        public function prepend(...$nodes): void {}

        /** @param DOMNode|string $nodes */
        public function replaceChildren(...$nodes): void {}

        public function insertAdjacentElement(string $where, DOMElement $element): ?DOMElement {}

        public function insertAdjacentText(string $where, string $data): void {}
    }

    class DOMDocument extends DOMNode implements DOMParentNode
    {
        /** @readonly */
        public ?DOMDocumentType $doctype;

        /** @readonly */
        public DOMImplementation $implementation;

        /** @readonly */
        public ?DOMElement $documentElement;

        /**
         * @readonly
         * @deprecated
         */
        public ?string $actualEncoding;

        public ?string $encoding;

        /** @readonly */
        public ?string $xmlEncoding;

        public bool $standalone;

        public bool $xmlStandalone;

        public ?string $version;

        public ?string $xmlVersion;

        public bool $strictErrorChecking;

        public ?string $documentURI;

        /**
         * @readonly
         * @deprecated
         */
        public mixed $config;

        public bool $formatOutput;

        public bool $validateOnParse;

        public bool $resolveExternals;

        public bool $preserveWhiteSpace;

        public bool $recover;

        public bool $substituteEntities;

        /** @readonly */
        public ?DOMElement $firstElementChild;

        /** @readonly */
        public ?DOMElement $lastElementChild;

        /** @readonly */
        public int $childElementCount;

        public function __construct(string $version = "1.0", string $encoding = "") {}

        /** @return DOMAttr|false */
        public function createAttribute(string $localName) {}

        /** @return DOMAttr|false */
        public function createAttributeNS(?string $namespace, string $qualifiedName) {}

        /** @return DOMCdataSection|false */
        public function createCDATASection(string $data) {}

        /** @tentative-return-type */
        public function createComment(string $data): DOMComment {}

        /** @tentative-return-type */
        public function createDocumentFragment(): DOMDocumentFragment {}

        /** @return DOMElement|false */
        public function createElement(string $localName, string $value = "")  {}

        /** @return DOMElement|false */
        public function createElementNS(?string $namespace, string $qualifiedName, string $value = "") {}

        /** @return DOMEntityReference|false */
        public function createEntityReference(string $name) {}

        /** @return DOMProcessingInstruction|false */
        public function createProcessingInstruction(string $target, string $data = "") {}

        /** @tentative-return-type */
        public function createTextNode(string $data): DOMText {}

        /** @tentative-return-type */
        public function getElementById(string $elementId): ?DOMElement {}

        /**
         * @tentative-return-type
         * @implementation-alias DOMElement::getElementsByTagName
         */
        public function getElementsByTagName(string $qualifiedName): DOMNodeList {}

        /**
         * @tentative-return-type
         * @implementation-alias DOMElement::getElementsByTagNameNS
         */
        public function getElementsByTagNameNS(?string $namespace, string $localName): DOMNodeList {}

        /** @return DOMNode|false */
        public function importNode(DOMNode $node, bool $deep = false) {}

        /** @tentative-return-type */
        public function load(string $filename, int $options = 0): bool {}

        /** @tentative-return-type */
        public function loadXML(string $source, int $options = 0): bool {}

        /** @tentative-return-type */
        public function normalizeDocument(): void {}

        /** @tentative-return-type */
        public function registerNodeClass(string $baseClass, ?string $extendedClass): true {}

        /** @tentative-return-type */
        public function save(string $filename, int $options = 0): int|false {}

#ifdef LIBXML_HTML_ENABLED
        /** @tentative-return-type */
        public function loadHTML(string $source, int $options = 0): bool {}

        /** @tentative-return-type */
        public function loadHTMLFile(string $filename, int $options = 0): bool {}

        /** @tentative-return-type */
        public function saveHTML(?DOMNode $node = null): string|false {}

        /** @tentative-return-type */
        public function saveHTMLFile(string $filename): int|false {}
#endif

        /** @tentative-return-type */
        public function saveXML(?DOMNode $node = null, int $options = 0): string|false {}

#ifdef LIBXML_SCHEMAS_ENABLED
        /** @tentative-return-type */
        public function schemaValidate(string $filename, int $flags = 0): bool {}

        /** @tentative-return-type */
        public function schemaValidateSource(string $source, int $flags = 0): bool {}

        /** @tentative-return-type */
        public function relaxNGValidate(string $filename): bool {}

        /** @tentative-return-type */
        public function relaxNGValidateSource(string $source): bool {}
#endif

        /** @tentative-return-type */
        public function validate(): bool {}

        /** @tentative-return-type */
        public function xinclude(int $options = 0): int|false {}

        /** @tentative-return-type */
        public function adoptNode(DOMNode $node): DOMNode|false {}

        /**
         * @param DOMNode|string $nodes
         * @implementation-alias DOMElement::append
         */
        public function append(...$nodes): void {}

        /**
         * @param DOMNode|string $nodes
         * @implementation-alias DOMElement::prepend
         */
        public function prepend(...$nodes): void {}

        /** @param DOMNode|string $nodes */
        public function replaceChildren(...$nodes): void {}
    }

    /** @alias DOM\DOMException */
    final class DOMException extends Exception
    {
        /**
         * Intentionally left untyped for BC reasons
         * @var int
         */
        public $code = 0; // TODO add proper type (i.e. int|string)
    }

    class DOMText extends DOMCharacterData
    {
        /** @readonly */
        public string $wholeText;

        public function __construct(string $data = "") {}

        /** @tentative-return-type */
        public function isWhitespaceInElementContent(): bool {}

        /**
         * @tentative-return-type
         * @alias DOMText::isWhitespaceInElementContent
         */
        public function isElementContentWhitespace(): bool {}

        /** @return DOMText|false */
        public function splitText(int $offset) {}
    }

    class DOMNamedNodeMap implements IteratorAggregate, Countable
    {
        /** @readonly */
        public int $length;

        /** @tentative-return-type */
        public function getNamedItem(string $qualifiedName): ?DOMNode {}

        /** @tentative-return-type */
        public function getNamedItemNS(?string $namespace, string $localName): ?DOMNode {}

        /** @tentative-return-type */
        public function item(int $index): ?DOMNode {}

        /** @tentative-return-type */
        public function count(): int {}

        public function getIterator(): Iterator {}
    }

    class DOMEntity extends DOMNode
    {
        /** @readonly */
        public ?string $publicId;

        /** @readonly */
        public ?string $systemId;

        /** @readonly */
        public ?string $notationName;

        /**
         * @readonly
         * @deprecated
         */
        public ?string $actualEncoding = null;

        /**
         * @readonly
         * @deprecated
         */
        public ?string $encoding = null;

        /**
         * @readonly
         * @deprecated
         */
        public ?string $version = null;
    }

    class DOMEntityReference extends DOMNode
    {
        public function __construct(string $name) {}
    }

    class DOMNotation extends DOMNode
    {
        /** @readonly */
        public string $publicId;

        /** @readonly */
        public string $systemId;
    }

    class DOMProcessingInstruction extends DOMNode
    {
        /** @readonly */
        public string $target;

        public string $data;

        public function __construct(string $name, string $value = "") {}
    }

#ifdef LIBXML_XPATH_ENABLED
    /** @not-serializable */
    class DOMXPath
    {
        /** @readonly */
        public DOMDocument $document;

        public bool $registerNodeNamespaces;

        public function __construct(DOMDocument $document, bool $registerNodeNS = true) {}

        /** @tentative-return-type */
        public function evaluate(string $expression, ?DOMNode $contextNode = null, bool $registerNodeNS = true): mixed {}

        /** @tentative-return-type */
        public function query(string $expression, ?DOMNode $contextNode = null, bool $registerNodeNS = true): mixed {}

        /** @tentative-return-type */
        public function registerNamespace(string $prefix, string $namespace): bool {}

        /** @tentative-return-type */
        public function registerPhpFunctions(string|array|null $restrict = null): void {}

        public function registerPhpFunctionNS(string $namespaceURI, string $name, callable $callable): void {}

        public static function quote(string $str): string {}
    }
#endif

    function dom_import_simplexml(object $node): DOMElement {}
}

namespace DOM
{
    /**
     * @var int
     * @cvalue INDEX_SIZE_ERR
     */
    const INDEX_SIZE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue DOMSTRING_SIZE_ERR
     */
    const STRING_SIZE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue HIERARCHY_REQUEST_ERR
     */
    const HIERARCHY_REQUEST_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue WRONG_DOCUMENT_ERR
     */
    const WRONG_DOCUMENT_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INVALID_CHARACTER_ERR
     */
    const INVALID_CHARACTER_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NO_DATA_ALLOWED_ERR
     */
    const NO_DATA_ALLOWED_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NO_MODIFICATION_ALLOWED_ERR
     */
    const NO_MODIFICATION_ALLOWED_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NOT_FOUND_ERR
     */
    const NOT_FOUND_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NOT_SUPPORTED_ERR
     */
    const NOT_SUPPORTED_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INUSE_ATTRIBUTE_ERR
     */
    const INUSE_ATTRIBUTE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INVALID_STATE_ERR
     */
    const INVALID_STATE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue SYNTAX_ERR
     */
    const SYNTAX_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INVALID_MODIFICATION_ERR
     */
    const INVALID_MODIFICATION_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue NAMESPACE_ERR
     */
    const NAMESPACE_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue INVALID_ACCESS_ERR
     */
    const INVALID_ACCESS_ERR = UNKNOWN;
    /**
     * @var int
     * @cvalue VALIDATION_ERR
     */
    const VALIDATION_ERR = UNKNOWN;

    /**
     * @var int
     * @cvalue DOM_HTML_NO_DEFAULT_NS
     */
    const HTML_NO_DEFAULT_NS = UNKNOWN;

    interface ParentNode
    {
        public function append(Node|string ...$nodes): void;
        public function prepend(Node|string ...$nodes): void;
        public function replaceChildren(Node|string ...$nodes): void;
    }

    interface ChildNode
    {
        public function remove(): void;
        public function before(Node|string ...$nodes): void;
        public function after(Node|string ...$nodes): void;
        public function replaceWith(Node|string ...$nodes): void;
    }

    /** @strict-properties */
    class Node
    {
        private final function __construct() {}

        /** @readonly */
        public int $nodeType;
        /** @readonly */
        public string $nodeName;

        /** @readonly */
        public ?string $baseURI;

        /** @readonly */
        public bool $isConnected;
        /** @readonly */
        public ?Document $ownerDocument;

        /** @implementation-alias DOMNode::getRootNode */
        public function getRootNode(array $options = []): Node {}
        /** @readonly */
        public ?Node $parentNode;
        /** @readonly */
        public ?Element $parentElement;
        /** @implementation-alias DOMNode::hasChildNodes */
        public function hasChildNodes(): bool {}
        /** @readonly */
        public NodeList $childNodes;
        /** @readonly */
        public ?Node $firstChild;
        /** @readonly */
        public ?Node $lastChild;
        /** @readonly */
        public ?Node $previousSibling;
        /** @readonly */
        public ?Node $nextSibling;

        public ?string $nodeValue;
        public ?string $textContent;
        /** @implementation-alias DOMNode::normalize */
        public function normalize(): void {}

        /** @implementation-alias DOMNode::cloneNode */
        public function cloneNode(bool $deep = false): Node {}
        public function isEqualNode(?Node $otherNode): bool {}
        public function isSameNode(?Node $otherNode): bool {}

        public const int DOCUMENT_POSITION_DISCONNECTED = 0x01;
        public const int DOCUMENT_POSITION_PRECEDING = 0x02;
        public const int DOCUMENT_POSITION_FOLLOWING = 0x04;
        public const int DOCUMENT_POSITION_CONTAINS = 0x08;
        public const int DOCUMENT_POSITION_CONTAINED_BY = 0x10;
        public const int DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC = 0x20;
        public function compareDocumentPosition(Node $other): int {}
        public function contains(?Node $other): bool {}

        public function lookupPrefix(?string $namespace): ?string {}
        /** @implementation-alias DOMNode::lookupNamespaceURI */
        public function lookupNamespaceURI(?string $prefix): ?string {}
        public function isDefaultNamespace(?string $namespace): bool {}

        public function insertBefore(Node $node, ?Node $child): Node {}
        public function appendChild(Node $node): Node {}
        public function replaceChild(Node $node, Node $child): Node {}
        public function removeChild(Node $child): Node {}

        /** @implementation-alias DOMNode::getLineNo */
        public function getLineNo(): int {}
        public function getNodePath(): string {}

        /** @implementation-alias DOMNode::C14N */
        public function C14N(bool $exclusive = false, bool $withComments = false, ?array $xpath = null, ?array $nsPrefixes = null): string|false {}
        /** @implementation-alias DOMNode::C14NFile */
        public function C14NFile(string $uri, bool $exclusive = false, bool $withComments = false, ?array $xpath = null, ?array $nsPrefixes = null): int|false {}

        /** @implementation-alias DOMNode::__sleep */
        public function __sleep(): array {}
        /** @implementation-alias DOMNode::__wakeup */
        public function __wakeup(): void {}
    }

    class NodeList implements IteratorAggregate, Countable
    {
        /** @readonly */
        public int $length;

        /**
         * @tentative-return-type
         * @implementation-alias DOMNodeList::count
         */
        public function count(): int {}

        /** @implementation-alias DOMNodeList::getIterator */
        public function getIterator(): \Iterator {}

        /* TODO: this probably needs fixes??? also via XPath can namespace nodes be returned? */
        /** @implementation-alias DOMNodeList::item */
        public function item(int $index): ?Node {}
    }

    class NamedNodeMap implements IteratorAggregate, Countable
    {
        /** @readonly */
        public int $length;

        /** @implementation-alias DOMNamedNodeMap::item */
        public function item(int $index): ?Attr {}
        /** @implementation-alias DOMNamedNodeMap::getNamedItem */
        public function getNamedItem(string $qualifiedName): ?Attr {}
        /** @implementation-alias DOMNamedNodeMap::getNamedItemNS */
        public function getNamedItemNS(?string $namespace, string $localName): ?Attr {}

        /**
         * @tentative-return-type
         * @implementation-alias DOMNamedNodeMap::count
         */
        public function count(): int {}

        /** @implementation-alias DOMNamedNodeMap::getIterator */
        public function getIterator(): \Iterator {}
    }

    class DTDNamedNodeMap implements IteratorAggregate, Countable
    {
        /** @readonly */
        public int $length;

        /** @implementation-alias DOMNamedNodeMap::item */
        public function item(int $index): Entity|Notation|null {}
        /** @implementation-alias DOMNamedNodeMap::getNamedItem */
        public function getNamedItem(string $qualifiedName): Entity|Notation|null {}
        /** @implementation-alias DOMNamedNodeMap::getNamedItemNS */
        public function getNamedItemNS(?string $namespace, string $localName): Entity|Notation|null {}

        /**
         * @tentative-return-type
         * @implementation-alias DOMNamedNodeMap::count
         */
        public function count(): int {}

        /** @implementation-alias DOMNamedNodeMap::getIterator */
        public function getIterator(): \Iterator {}
    }

    class HTMLCollection implements IteratorAggregate, Countable
    {
        /** @readonly */
        public int $length;

        /** @implementation-alias DOMNodeList::item */
        public function item(int $index): ?Element {}

        /* TODO: implement namedItem */

        /**
         * @tentative-return-type
         * @implementation-alias DOMNodeList::count
         */
        public function count(): int {}

        /** @implementation-alias DOMNodeList::getIterator */
        public function getIterator(): \Iterator {}
    }

    class Element extends Node implements ParentNode, ChildNode
    {
        /** @readonly */
        public ?string $namespaceURI;
        /** @readonly */
        public ?string $prefix;
        /** @readonly */
        public string $localName;
        /** @readonly */
        public string $tagName;

        public string $id;
        public string $className;

        /** @implementation-alias DOMNode::hasAttributes */
        public function hasAttributes(): bool {}
        /** @readonly */
        public NamedNodeMap $attributes;
        /** @implementation-alias DOMElement::getAttributeNames */
        public function getAttributeNames(): array {}
        /** @implementation-alias DOMElement::getAttribute */
        public function getAttribute(string $qualifiedName): ?string {}
        /** @implementation-alias DOMElement::getAttributeNS */
        public function getAttributeNS(?string $namespace, string $localName): ?string {}
        /** @implementation-alias DOMElement::setAttribute */
        public function setAttribute(string $qualifiedName, string $value): void {}
        /** @implementation-alias DOMElement::setAttributeNS */
        public function setAttributeNS(?string $namespace, string $qualifiedName, string $value): void {}
        public function removeAttribute(string $qualifiedName): void {}
        /** @implementation-alias DOMElement::removeAttributeNS */
        public function removeAttributeNS(?string $namespace, string $localName): void {}
        /** @implementation-alias DOMElement::toggleAttribute */
        public function toggleAttribute(string $qualifiedName, ?bool $force = null): bool {}
        /** @implementation-alias DOMElement::hasAttribute */
        public function hasAttribute(string $qualifiedName): bool {}
        /** @implementation-alias DOMElement::hasAttributeNS */
        public function hasAttributeNS(?string $namespace, string $localName): bool {}

        /** @implementation-alias DOMElement::getAttributeNode */
        public function getAttributeNode(string $qualifiedName): ?Attr {}
        /** @implementation-alias DOMElement::getAttributeNodeNS */
        public function getAttributeNodeNS(?string $namespace, string $localName): ?Attr {}
        /** @implementation-alias DOM\Element::setAttributeNodeNS */
        public function setAttributeNode(Attr $attr) : ?Attr {}
        public function setAttributeNodeNS(Attr $attr) : ?Attr {}
        public function removeAttributeNode(Attr $attr) : Attr {}

        public function getElementsByTagName(string $qualifiedName): HTMLCollection {}
        public function getElementsByTagNameNS(?string $namespace, string $localName): HTMLCollection {}

        public function insertAdjacentElement(string $where, Element $element): ?Element {}
        /** @implementation-alias DOMElement::insertAdjacentText */
        public function insertAdjacentText(string $where, string $data): void {}

        /** @readonly */
        public ?Element $firstElementChild;
        /** @readonly */
        public ?Element $lastElementChild;
        /** @readonly */
        public int $childElementCount;
        /** @readonly */
        public ?Element $previousElementSibling;
        /** @readonly */
        public ?Element $nextElementSibling;

        /** @implementation-alias DOMElement::setIdAttribute */
        public function setIdAttribute(string $qualifiedName, bool $isId): void {}
        /** @implementation-alias DOMElement::setIdAttributeNS */
        public function setIdAttributeNS(?string $namespace, string $qualifiedName, bool $isId): void {}
        public function setIdAttributeNode(Attr $attr, bool $isId): void {}

        /** @implementation-alias DOMElement::remove */
        public function remove(): void {}
        /** @implementation-alias DOMElement::before */
        public function before(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::after */
        public function after(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::replaceWith */
        public function replaceWith(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::append */
        public function append(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::prepend */
        public function prepend(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::replaceChildren */
        public function replaceChildren(Node|string ...$nodes): void {}
    }

    class Attr extends Node
    {
        /** @readonly */
        public ?string $namespaceURI;
        /** @readonly */
        public ?string $prefix;
        /** @readonly */
        public string $localName;
        /** @readonly */
        public string $name;
        public string $value;

        /** @readonly */
        public ?Element $ownerElement;

        /** @readonly */
        public bool $specified = true;

        /** @implementation-alias DOMAttr::isId */
        public function isId(): bool {}
    }

    class CharacterData extends Node implements ChildNode
    {
        /** @readonly */
        public ?Element $previousElementSibling;
        /** @readonly */
        public ?Element $nextElementSibling;

        public string $data;
        /** @readonly */
        public int $length;
        /** @implementation-alias DOMCharacterData::substringData */
        public function substringData(int $offset, int $count): string {}
        public function appendData(string $data): void {}
        public function insertData(int $offset, string $data): void {}
        public function deleteData(int $offset, int $count): void {}
        public function replaceData(int $offset, int $count, string $data): void {}

        /** @implementation-alias DOMElement::remove */
        public function remove(): void {}
        /** @implementation-alias DOMElement::before */
        public function before(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::after */
        public function after(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::replaceWith */
        public function replaceWith(Node|string ...$nodes): void {}
    }

    class Text extends CharacterData
    {
        /* No constructor because Node has a final private constructor, so PHP does not allow overriding that. */

        /** @implementation-alias DOMText::splitText */
        public function splitText(int $offset): Text {}
        /** @readonly */
        public string $wholeText;
    }

    class CDATASection extends Text {}

    class ProcessingInstruction extends CharacterData
    {
        /** @readonly */
        public string $target;
    }

    class Comment extends CharacterData
    {
        /* No constructor because Node has a final private constructor, so PHP does not allow overriding that. */
    }

    class DocumentType extends Node implements ChildNode
    {
        /** @readonly */
        public string $name;
        /** @readonly */
        public DTDNamedNodeMap $entities;
        /** @readonly */
        public DTDNamedNodeMap $notations;
        /** @readonly */
        public string $publicId;
        /** @readonly */
        public string $systemId;
        /** @readonly */
        public ?string $internalSubset;

        /** @implementation-alias DOMElement::remove */
        public function remove(): void {}
        /** @implementation-alias DOMElement::before */
        public function before(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::after */
        public function after(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::replaceWith */
        public function replaceWith(Node|string ...$nodes): void {}
    }

    class DocumentFragment extends Node implements ParentNode
    {
        /** @readonly */
        public ?Element $firstElementChild;
        /** @readonly */
        public ?Element $lastElementChild;
        /** @readonly */
        public int $childElementCount;

        /** @implementation-alias DOMDocumentFragment::appendXML */
        public function appendXML(string $data): bool {}
        /** @implementation-alias DOMElement::append */
        public function append(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::prepend */
        public function prepend(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::replaceChildren */
        public function replaceChildren(Node|string ...$nodes): void {}
    }

    class Entity extends Node
    {
        /** @readonly */
        public ?string $publicId;
        /** @readonly */
        public ?string $systemId;
        /** @readonly */
        public ?string $notationName;
    }

    class EntityReference extends Node {}

    class Notation extends Node
    {
        /** @readonly */
        public string $publicId;
        /** @readonly */
        public string $systemId;
    }

    abstract class Document extends Node implements ParentNode
    {
        /** @readonly */
        public string $URL;
        /** @readonly */
        public string $documentURI;
        public string $characterSet;
        public string $charset;
        public string $inputEncoding;

        /** @readonly */
        public ?DocumentType $doctype;
        /** @readonly */
        public ?Element $documentElement;
        /** @implementation-alias DOM\Element::getElementsByTagName */
        public function getElementsByTagName(string $qualifiedName): HTMLCollection {}
        /** @implementation-alias DOM\Element::getElementsByTagNameNS */
        public function getElementsByTagNameNS(?string $namespace, string $localName): HTMLCollection {}

        public function createElement(string $localName): Element {}
        public function createElementNS(?string $namespace, string $qualifiedName): Element {}
        /** @implementation-alias DOMDocument::createDocumentFragment */
        public function createDocumentFragment(): DocumentFragment {}
        /** @implementation-alias DOMDocument::createTextNode */
        public function createTextNode(string $data): Text {}
        /** @implementation-alias DOMDocument::createCDATASection */
        public function createCDATASection(string $data): CDATASection {}
        /** @implementation-alias DOMDocument::createComment */
        public function createComment(string $data): Comment {}
        public function createProcessingInstruction(string $target, string $data): ProcessingInstruction {}

        public function importNode(?Node $node, bool $deep = false): Node {}
        public function adoptNode(Node $node): Node {}

        /** @implementation-alias DOMDocument::createAttribute */
        public function createAttribute(string $localName): Attr {}
        /** @implementation-alias DOMDocument::createAttributeNS */
        public function createAttributeNS(?string $namespace, string $qualifiedName): Attr {}

        /** @readonly */
        public ?Element $firstElementChild;
        /** @readonly */
        public ?Element $lastElementChild;
        /** @readonly */
        public int $childElementCount;

        /** @implementation-alias DOMDocument::getElementById */
        public function getElementById(string $elementId): ?Element {}

        public function registerNodeClass(string $baseClass, ?string $extendedClass): void {}

#ifdef LIBXML_SCHEMAS_ENABLED
        /** @implementation-alias DOMDocument::schemaValidate */
        public function schemaValidate(string $filename, int $flags = 0): bool {}
        /** @implementation-alias DOMDocument::schemaValidateSource */
        public function schemaValidateSource(string $source, int $flags = 0): bool {}
        /** @implementation-alias DOMDocument::relaxNGValidate */
        public function relaxNGValidate(string $filename): bool {}
        /** @implementation-alias DOMDocument::relaxNGValidateSource */
        public function relaxNGValidateSource(string $source): bool {}
#endif

        /** @implementation-alias DOMElement::append */
        public function append(Node|string ...$nodes): void {}
        /** @implementation-alias DOMElement::prepend */
        public function prepend(Node|string ...$nodes): void {}
        /** @implementation-alias DOMDocument::replaceChildren */
        public function replaceChildren(Node|string ...$nodes): void {}

        public function importLegacyNode(\DOMNode $node, bool $deep = false): Node {}
    }

    final class HTMLDocument extends Document
    {
        public static function createEmpty(string $encoding = "UTF-8"): HTMLDocument {}

        public static function createFromFile(string $path, int $options = 0, ?string $overrideEncoding = null): HTMLDocument {}

        public static function createFromString(string $source, int $options = 0, ?string $overrideEncoding = null): HTMLDocument {}

        /** @implementation-alias DOM\XMLDocument::saveXML */
        public function saveXML(?Node $node = null, int $options = 0): string|false {}

        /** @implementation-alias DOMDocument::save */
        public function saveXMLFile(string $filename, int $options = 0): int|false {}

        public function saveHTML(?Node $node = null): string|false {}

        public function saveHTMLFile(string $filename): int|false {}
    }

    final class XMLDocument extends Document
    {
        public static function createEmpty(string $version = "1.0", string $encoding = "UTF-8"): XMLDocument {}

        public static function createFromFile(string $path, int $options = 0, ?string $overrideEncoding = null): XMLDocument {}

        public static function createFromString(string $source, int $options = 0, ?string $overrideEncoding = null): XMLDocument {}

        /** @readonly */
        public string $xmlEncoding;

        public bool $xmlStandalone;

        public string $xmlVersion;

        public bool $formatOutput;

        /** @implementation-alias DOMDocument::createEntityReference */
        public function createEntityReference(string $name): EntityReference {}

        /** @implementation-alias DOMDocument::validate */
        public function validate(): bool {}

        /** @implementation-alias DOMDocument::xinclude */
        public function xinclude(int $options = 0): int|false {}

        public function saveXML(?Node $node = null, int $options = 0): string|false {}

        /** @implementation-alias DOMDocument::save */
        public function saveXMLFile(string $filename, int $options = 0): int|false {}
    }

#ifdef LIBXML_XPATH_ENABLED
    /** @not-serializable */
    final class XPath
    {
        /* TODO: return type of evaluated/query can return dom namespace node??? */
        /* TODO: get rid of mixed pls... */
        /* TODO: double check return types (e.g. nodelist) */

        /** @readonly */
        public DOM\Document $document;

        public bool $registerNodeNamespaces;

        public function __construct(Document $document, bool $registerNodeNS = true) {}

        public function evaluate(string $expression, ?DOM\Node $contextNode = null, bool $registerNodeNS = true): mixed {}

        public function query(string $expression, ?DOM\Node $contextNode = null, bool $registerNodeNS = true): mixed {}

        /** @implementation-alias DOMXPath::registerNamespace */
        public function registerNamespace(string $prefix, string $namespace): bool {}

        /** @implementation-alias DOMXPath::registerPhpFunctions */
        public function registerPhpFunctions(string|array|null $restrict = null): void {}

        /** @implementation-alias DOMXPath::registerPhpFunctionNS */
        public function registerPhpFunctionNS(string $namespaceURI, string $name, callable $callable): void {}
    }
#endif

    function import_simplexml(object $node): Element {}
}
