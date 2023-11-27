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

    /** @alias DOM\DocumentType */
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

    /** @alias DOM\CDATASection */
    class DOMCdataSection extends DOMText
    {
        public function __construct(string $data) {}
    }

    /** @alias DOM\Comment */
    class DOMComment extends DOMCharacterData
    {
        public function __construct(string $data = "") {}
    }

    /** @alias DOM\ParentNode */
    interface DOMParentNode
    {
        /** @param DOMNode|string $nodes */
        public function append(...$nodes): void;

        /** @param DOMNode|string $nodes */
        public function prepend(...$nodes): void;

        /** @param DOMNode|string $nodes */
        public function replaceChildren(...$nodes): void;
    }

    /** @alias DOM\ChildNode */
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

    /** @alias DOM\Node */
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
        public ?DOM\Document $ownerDocument;

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

    /** @alias DOM\NameSpaceNode */
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

        /** @return DOMDocument|false */
        public function createDocument(?string $namespace = null, string $qualifiedName = "", ?DOMDocumentType $doctype = null) {}
    }

    /** @alias DOM\DocumentFragment */
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
         * @implementation-alias DOM\Document::replaceChildren
         */
        public function replaceChildren(...$nodes): void {}
    }

    /** @alias DOM\NodeList */
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

    /** @alias DOM\CharacterData */
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

    /** @alias DOM\Attr */
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

    /** @alias DOM\Element */
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
        public function setAttribute(string $qualifiedName, string $value) {} // TODO return type shouldn't depend on the call scope

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

    class DOMDocument extends DOM\Document
    {
        /** @readonly */
        public DOMImplementation $implementation;

        /**
         * @readonly
         * @deprecated
         */
        public ?string $actualEncoding;

        /** @readonly */
        public ?string $xmlEncoding;

        public bool $standalone;

        public bool $xmlStandalone;

        public ?string $version;

        public ?string $xmlVersion;

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

        public function __construct(string $version = "1.0", string $encoding = "") {}

        /** @return DOMEntityReference|false */
        public function createEntityReference(string $name) {}

        /** @tentative-return-type */
        public function load(string $filename, int $options = 0): bool {}

        /** @tentative-return-type */
        public function loadXML(string $source, int $options = 0): bool {}

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

        /** @tentative-return-type */
        public function validate(): bool {}

        /** @tentative-return-type */
        public function xinclude(int $options = 0): int|false {}
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

    /** @alias DOM\Text */
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

    /** @alias DOM\NamedNodeMap */
    class DOMNamedNodeMap implements IteratorAggregate, Countable
    {
        /** @readonly */
        public int $length;

        /** @tentative-return-type */
        public function getNamedItem(string $qualifiedName): ?DOMNode {} // TODO DOM spec returns DOMAttr

        /** @tentative-return-type */
        public function getNamedItemNS(?string $namespace, string $localName): ?DOMNode {} // TODO DOM spec returns DOMAttr

        /** @tentative-return-type */
        public function item(int $index): ?DOMNode {} // TODO DOM spec returns DOMAttr

        /** @tentative-return-type */
        public function count(): int {}

        public function getIterator(): Iterator {}
    }

    /** @alias DOM\Entity */
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

    /** @alias DOM\EntityReference */
    class DOMEntityReference extends DOMNode
    {
        public function __construct(string $name) {}
    }

    /** @alias DOM\Notation */
    class DOMNotation extends DOMNode
    {
        /** @readonly */
        public string $publicId;

        /** @readonly */
        public string $systemId;
    }

    /** @alias DOM\ProcessingInstruction */
    class DOMProcessingInstruction extends DOMNode
    {
        /** @readonly */
        public string $target;

        public string $data;

        public function __construct(string $name, string $value = "") {}
    }

#ifdef LIBXML_XPATH_ENABLED
    /**
     * @not-serializable
     * @alias DOM\XPath
     */
    class DOMXPath
    {
        /** @readonly */
        public DOM\Document $document;

        public bool $registerNodeNamespaces;

        public function __construct(DOM\Document $document, bool $registerNodeNS = true) {}

        /** @tentative-return-type */
        public function evaluate(string $expression, ?DOMNode $contextNode = null, bool $registerNodeNS = true): mixed {}

        /** @tentative-return-type */
        public function query(string $expression, ?DOMNode $contextNode = null, bool $registerNodeNS = true): mixed {}

        /** @tentative-return-type */
        public function registerNamespace(string $prefix, string $namespace): bool {}

        /** @tentative-return-type */
        public function registerPhpFunctions(string|array|null $restrict = null): void {}
    }
#endif

    function dom_import_simplexml(object $node): DOMElement {}
}

namespace DOM
{
    /**
     * @var int
     * @cvalue PHP_ERR
     */
    const PHP_ERR = UNKNOWN;
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

    abstract class Document extends \DOMNode implements \DOMParentNode
    {
        /** @readonly */
        public ?DocumentType $doctype;

        /** @readonly */
        public ?Element $documentElement;

        public ?string $encoding;

        public bool $strictErrorChecking;

        public ?string $documentURI;

        /** @readonly */
        public ?Element $firstElementChild;

        /** @readonly */
        public ?Element $lastElementChild;

        /** @readonly */
        public int $childElementCount;

        /** @return Attr|false */
        public function createAttribute(string $localName) {}

        /** @return Attr|false */
        public function createAttributeNS(?string $namespace, string $qualifiedName) {}

        /** @return CDATASection|false */
        public function createCDATASection(string $data) {}

        /** @tentative-return-type */
        public function createComment(string $data): Comment {}

        /** @tentative-return-type */
        public function createDocumentFragment(): DocumentFragment {}

        /** @return Element|false */
        public function createElement(string $localName, string $value = "")  {}

        /** @return Element|false */
        public function createElementNS(?string $namespace, string $qualifiedName, string $value = "") {}

        /** @return ProcessingInstruction|false */
        public function createProcessingInstruction(string $target, string $data = "") {}

        /** @tentative-return-type */
        public function createTextNode(string $data): Text {}

        /** @tentative-return-type */
        public function getElementById(string $elementId): ?Element {}

        /** @tentative-return-type */
        public function getElementsByTagName(string $qualifiedName): NodeList {}

        /** @tentative-return-type */
        public function getElementsByTagNameNS(?string $namespace, string $localName): NodeList {}

        /** @return Node|false */
        public function importNode(Node $node, bool $deep = false) {}

        /** @tentative-return-type */
        public function normalizeDocument(): void {}

        /** @tentative-return-type */
        public function registerNodeClass(string $baseClass, ?string $extendedClass): bool {}

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
        public function adoptNode(Node $node): Node|false {}

        /**
         * @param Node|string $nodes
         * @implementation-alias DOMElement::append
         */
        public function append(...$nodes): void {}

        /**
         * @param Node|string $nodes
         * @implementation-alias DOMElement::prepend
         */
        public function prepend(...$nodes): void {}

        /** @param Node|string $nodes */
        public function replaceChildren(...$nodes): void {}
    }

    /** @strict-properties */
    final class HTMLDocument extends Document
    {
        private function __construct() {}

        public static function createEmpty(string $encoding = "UTF-8"): HTMLDocument {}

        public static function createFromFile(string $path, int $options = 0, ?string $overrideEncoding = null): HTMLDocument {}

        public static function createFromString(string $source, int $options = 0, ?string $overrideEncoding = null): HTMLDocument {}

        /** @implementation-alias DOMDocument::saveXML */
        public function saveXML(?Node $node = null, int $options = 0): string|false {}

        /** @implementation-alias DOMDocument::save */
        public function saveXMLFile(string $filename, int $options = 0): int|false {}

        public function saveHTML(?Node $node = null): string|false {}

        public function saveHTMLFile(string $filename): int|false {}
    }

    /** @strict-properties */
    final class XMLDocument extends Document
    {
        /** @implementation-alias DOM\HTMLDocument::__construct */
        private function __construct() {}

        public static function createEmpty(string $version = "1.0", string $encoding = "UTF-8"): XMLDocument {}

        public static function createFromFile(string $path, int $options = 0, ?string $overrideEncoding = null): XMLDocument {}

        public static function createFromString(string $source, int $options = 0, ?string $overrideEncoding = null): XMLDocument {}

        /** @readonly */
        public ?string $xmlEncoding;

        public bool $standalone;

        public bool $xmlStandalone;

        public ?string $version;

        public ?string $xmlVersion;

        public bool $formatOutput;

        public bool $validateOnParse;

        public bool $resolveExternals;

        public bool $preserveWhiteSpace;

        public bool $recover;

        public bool $substituteEntities;

        /**
         * @implementation-alias DOMDocument::createEntityReference
         * @return EntityReference|false
         */
        public function createEntityReference(string $name) {}

        /**
         * @implementation-alias DOMDocument::validate
         */
        public function validate(): bool {}

        /**
         * @tentative-return-type
         * @implementation-alias DOMDocument::xinclude
         */
        public function xinclude(int $options = 0): int|false {}

        /**
         * @tentative-return-type
         * @implementation-alias DOMDocument::saveXML
         */
        public function saveXML(?Node $node = null, int $options = 0): string|false {}

        /**
         * @tentative-return-type
         * @implementation-alias DOMDocument::save
         */
        public function saveXMLFile(string $filename, int $options = 0): int|false {}
    }

    /** @implementation-alias dom_import_simplexml */
    function import_simplexml(object $node): Element {}
}
