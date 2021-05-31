<?php

/** @generate-class-entries */

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
    /** @readonly */
    public string $nodeName;

    public ?string $nodeValue;

    /** @readonly */
    public int $nodeType;

    /** @readonly */
    public ?DOMNode $parentNode;

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

    /** @return string|false */
    public function C14N(bool $exclusive = false, bool $withComments = false, ?array $xpath = null, ?array $nsPrefixes = null) {}

    /** @return int|false */
    public function C14NFile(string $uri, bool $exclusive = false, bool $withComments = false, ?array $xpath = null, ?array $nsPrefixes = null) {}

    /** @return DOMNode|false */
    public function cloneNode(bool $deep = false) {}

    /** @return int */
    public function getLineNo() {}

    /** @return string|null */
    public function getNodePath() {}

    /** @return bool */
    public function hasAttributes() {}

    /** @return bool */
    public function hasChildNodes() {}

    /** @return DOMNode|false */
    public function insertBefore(DOMNode $node, ?DOMNode $child = null) {}

    /** @return bool */
    public function isDefaultNamespace(string $namespace) {}

    /** @return bool */
    public function isSameNode(DOMNode $otherNode) {}

    /** @return bool */
    public function isSupported(string $feature, string $version) {}

    /** @return string|null */
    public function lookupNamespaceURI(?string $prefix) {}

    /** @return string|null */
    public function lookupPrefix(string $namespace) {}

    /** @return void */
    public function normalize() {}

    /** @return DOMNode|false */
    public function removeChild(DOMNode $child) {}

    /** @return DOMNode|false */
    public function replaceChild(DOMNode $node, DOMNode $child) {}
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
    public ?DOMDocument $ownerDocument;

    /** @readonly */
    public ?DOMNode $parentNode;
}

class DOMImplementation
{
    /** @return void */
    public function getFeature(string $feature, string $version) {}

    /** @return bool */
    public function hasFeature(string $feature, string $version) {}

    /** @return DOMDocumentType|false */
    public function createDocumentType(string $qualifiedName, string $publicId = "", string $systemId = "") {}

    /** @return DOMDocument|false */
    public function createDocument(?string $namespace = null, string $qualifiedName = "", ?DOMDocumentType $doctype = null) {}
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

    /** @return bool */
    public function appendXML(string $data) {}

    /** @param DOMNode|string $nodes */
    public function append(...$nodes): void {}

    /** @param DOMNode|string $nodes */
    public function prepend(...$nodes): void {}
}

class DOMNodeList implements IteratorAggregate, Countable
{
    /** @readonly */
    public int $length;

    /** @return int|false */
    public function count() {}

    public function getIterator(): Iterator {}

    /** @return DOMNode|null */
    public function item(int $index) {}
}

class DOMCharacterData extends DOMNode implements DOMChildNode
{
    /** @readonly */
    public string $data;

    /** @readonly */
    public int $length;

    /** @readonly */
    public ?DOMElement $previousElementSibling;

    /** @readonly */
    public ?DOMElement $nextElementSibling;

    /** @return bool */
    public function appendData(string $data) {}

    /** @return string|false */
    public function substringData(int $offset, int $count) {}

    /** @return bool */
    public function insertData(int $offset, string $data) {}

    /** @return bool */
    public function deleteData(int $offset, int $count) {}

    /** @return bool */
    public function replaceData(int $offset, int $count, string $data) {}

    /** @param DOMNode|string $nodes */
    public function replaceWith(...$nodes): void {}

    public function remove(): void {}

    /** @param DOMNode|string $nodes */
    public function before(... $nodes): void {}

    /** @param DOMNode|string $nodes */
    public function after(...$nodes): void {}
}

class DOMAttr extends DOMNode
{
    /** @readonly */
    public string $name;

    /** @readonly */
    public bool $specified = true;

    /** @readonly */
    public string $value;

    /** @readonly */
    public ?DOMElement $ownerElement;

    /** @readonly */
    public mixed $schemaTypeInfo = null;

    public function __construct(string $name, string $value = "") {}

    /** @return bool */
    public function isId() {}
}

class DOMElement extends DOMNode implements DOMParentNode, DOMChildNode
{
    /** @readonly */
    public string $tagName;

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

    /** @return string */
    public function getAttribute(string $qualifiedName) {}

    /** @return string */
    public function getAttributeNS(?string $namespace, string $localName) {}

    /** @return DOMAttr|DOMNamespaceNode|false */
    public function getAttributeNode(string $qualifiedName) {}

    /** @return DOMAttr|DOMNamespaceNode|null */
    public function getAttributeNodeNS(?string $namespace, string $localName) {}

    /** @return DOMNodeList */
    public function getElementsByTagName(string $qualifiedName) {}

    /** @return DOMNodeList */
    public function getElementsByTagNameNS(?string $namespace, string $localName) {}

    /** @return bool */
    public function hasAttribute(string $qualifiedName) {}

    /** @return bool */
    public function hasAttributeNS(?string $namespace, string $localName) {}

    /** @return bool */
    public function removeAttribute(string $qualifiedName) {}

    /** @return void */
    public function removeAttributeNS(?string $namespace, string $localName) {}

    /** @return DOMAttr|false */
    public function removeAttributeNode(DOMAttr $attr) {}

    /** @return DOMAttr|bool */
    public function setAttribute(string $qualifiedName, string $value) {}

    /** @return void */
    public function setAttributeNS(?string $namespace, string $qualifiedName, string $value) {}

    /** @return DOMAttr|null|false */
    public function setAttributeNode(DOMAttr $attr) {}

    /** @return DOMAttr|null|false */
    public function setAttributeNodeNS(DOMAttr $attr) {}

    /** @return void */
    public function setIdAttribute(string $qualifiedName, bool $isId) {}

    /** @return void */
    public function setIdAttributeNS(string $namespace, string $qualifiedName, bool $isId) {}

    /** @return void */
    public function setIdAttributeNode(DOMAttr $attr, bool $isId) {}

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
    public mixed $config = null;

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

    /** @return DOMComment|false */
    public function createComment(string $data) {}

    /** @return DOMDocumentFragment|false */
    public function createDocumentFragment() {}

    /** @return DOMElement|false */
    public function createElement(string $localName, string $value = "") {}

    /** @return DOMElement|false */
    public function createElementNS(?string $namespace, string $qualifiedName, string $value = "") {}

    /** @return DOMEntityReference|false */
    public function createEntityReference(string $name) {}

    /** @return DOMProcessingInstruction|false */
    public function createProcessingInstruction(string $target, string $data = "") {}

    /** @return DOMText|false */
    public function createTextNode(string $data) {}

    /** @return DOMElement|null */
    public function getElementById(string $elementId) {}

    /** @return DOMNodeList */
    public function getElementsByTagName(string $qualifiedName) {}

    /** @return DOMNodeList */
    public function getElementsByTagNameNS(?string $namespace, string $localName) {}

    /** @return DOMNode|false */
    public function importNode(DOMNode $node, bool $deep = false) {}

    /** @return DOMDocument|bool */
    public function load(string $filename, int $options = 0) {}

    /** @return DOMDocument|bool */
    public function loadXML(string $source, int $options = 0) {}

    /** @return void */
    public function normalizeDocument() {}

    /** @return bool */
    public function registerNodeClass(string $baseClass, ?string $extendedClass) {}

    /** @return int|false */
    public function save(string $filename, int $options = 0) {}

#ifdef LIBXML_HTML_ENABLED
    /** @return DOMDocument|bool */
    public function loadHTML(string $source, int $options = 0) {}

    /** @return DOMDocument|bool */
    public function loadHTMLFile(string $filename, int $options = 0) {}

    /** @return string|false */
    public function saveHTML(?DOMNode $node = null) {}

    /** @return int|false */
    public function saveHTMLFile(string $filename) {}
#endif

    /** @return string|false */
    public function saveXML(?DOMNode $node = null, int $options = 0) {}

#ifdef LIBXML_SCHEMAS_ENABLED
    /** @return bool */
    public function schemaValidate(string $filename, int $flags = 0) {}

    /** @return bool */
    public function schemaValidateSource(string $source, int $flags = 0) {}

    /** @return bool */
    public function relaxNGValidate(string $filename) {}

    /** @return bool */
    public function relaxNGValidateSource(string $source) {}
#endif

    /** @return bool */
    public function validate() {}

    /** @return int|false */
    public function xinclude(int $options = 0) {}

    /** @return DOMNode|false */
    public function adoptNode(DOMNode $node) {}

    /** @param DOMNode|string $nodes */
    public function append(...$nodes): void {}

    /** @param DOMNode|string $nodes */
    public function prepend(...$nodes): void {}
}

final class DOMException extends Exception
{
    /** @var int Intentionally left untyped */
    public $code = 0;
}

class DOMText extends DOMCharacterData
{
    /** @readonly */
    public string $wholeText;

    public function __construct(string $data = "") {}

    /** @return bool */
    public function isWhitespaceInElementContent() {}

    /**
     * @return bool
     * @alias DOMText::isWhitespaceInElementContent
     */
    public function isElementContentWhitespace() {}

    /** @return DOMText|false */
    public function splitText(int $offset) {}
}

class DOMNamedNodeMap implements IteratorAggregate, Countable
{
    /** @readonly */
    public int $length;

    /** @return DOMNode|null */
    public function getNamedItem(string $qualifiedName) {}

    /** @return DOMNode|null */
    public function getNamedItemNS(?string $namespace, string $localName) {}

    /** @return DOMNode|null */
    public function item(int $index) {}

    /** @return int|false */
    public function count() {}

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

    /** @readonly */
    public mixed $actualEncoding = null;

    /** @readonly */
    public mixed $encoding = null;

    /** @readonly */
    public mixed $version = null;
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
class DOMXPath
{
    /** @readonly */
    public DOMDocument $document;

    public bool $registerNodeNamespaces;

    public function __construct(DOMDocument $document, bool $registerNodeNS = true) {}

    /** @return mixed */
    public function evaluate(string $expression, ?DOMNode $contextNode = null, bool $registerNodeNS = true) {}

    /** @return mixed */
    public function query(string $expression, ?DOMNode $contextNode = null, bool $registerNodeNS = true) {}

    /** @return bool */
    public function registerNamespace(string $prefix, string $namespace) {}

    /** @return void */
    public function registerPhpFunctions(string|array|null $restrict = null) {}
}
#endif

function dom_import_simplexml(object $node): ?DOMElement {}
