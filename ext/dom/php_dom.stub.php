<?php

/** @generate-function-entries */

class DOMDocumentType
{
}

class DOMCdataSection
{
    public function __construct(string $value) {}
}

class DOMComment
{
    public function __construct(string $value = "") {}
}

interface DOMParentNode
{
    /** @var ...DOMNode|string $nodes */
    public function append(...$nodes): void;

    /** @var ...DOMNode|string $nodes */
    public function prepend(...$nodes): void;
}

interface DOMChildNode
{
    public function remove(): void;

    /** @var ...DOMNode|string $nodes */
    public function before(... $nodes): void;

    /** @var ...DOMNode|string $nodes */
    public function after(...$nodes): void;

    /** @var ...DOMNode|string $nodes */
    public function replaceWith(...$nodes): void;
}

class DOMNode
{
    /** @return DOMNode|false */
    public function appendChild(DOMNode $newChild) {}

    /** @return string|false */
    public function C14N(bool $exclusive = false, bool $with_comments = false, ?array $xpath = null, ?array $ns_prefixes = null) {}

    /** @return int|false */
    public function C14NFile(string $uri, bool $exclusive = false, bool $with_comments = false, ?array $xpath = null, ?array $ns_prefixes = null) {}

    /** @return DOMNode|false */
    public function cloneNode(bool $recursive = false) {}

    /** @return int */
    public function getLineNo() {}

    /** @return string|null
     */
    public function getNodePath() {}

    /** @return bool */
    public function hasAttributes() {}

    /** @return bool */
    public function hasChildNodes() {}

    /** @return DOMNode|false */
    public function insertBefore(DOMNode $newChild, ?DOMNode $refChild = null) {}

    /** @return bool */
    public function isDefaultNamespace(string $namespaceURI) {}

    /** @return bool */
    public function isSameNode(DOMNode $other) {}

    /** @return bool */
    public function isSupported(string $feature, string $version) {}

    /** @return string|null */
    public function lookupNamespaceURI(?string $prefix) {}

    /** @return string|null */
    public function lookupPrefix(string $namespaceURI) {}

    /** @return void */
    public function normalize() {}

    /** @return DOMNode|false */
    public function removeChild(DOMNode $oldChild) {}

    /** @return DOMNode|false */
    public function replaceChild(DOMNode $newChild, DOMNode $oldChild) {}
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
    public function createDocument(string $namespaceURI = "", string $qualifiedName = "", ?DOMDocumentType $doctype = null) {}
}

class DOMDocumentFragment implements DOMParentNode
{
    public function __construct() {}

    /** @return bool */
    public function appendXML(string $data) {}

    /** @var ...DOMNode|string $nodes */
    public function append(...$nodes): void {}

    /** @var ...DOMNode|string $nodes */
    public function prepend(...$nodes): void {}
}

class DOMNodeList implements IteratorAggregate, Countable
{
    /** @return int|false */
    public function count() {}

    public function getIterator(): Iterator {}

    /** @return ?DOMNode */
    public function item(int $index) {}
}

class DOMCharacterData implements DOMChildNode
{
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

    /** @var ...DOMNode|string $nodes */
    public function replaceWith(...$nodes): void {}

    public function remove(): void {}

    /** @var ...DOMNode|string $nodes */
    public function before(... $nodes): void {}

    /** @var ...DOMNode|string $nodes */
    public function after(...$nodes): void {}
}

class DOMAttr
{
    public function __construct(string $name, string $value = "") {}

    /** @return bool */
    public function isId() {}
}

class DOMElement implements DOMParentNode, DOMChildNode
{
    public function __construct(string $name, ?string $value = null, string $uri = "") {}

    /** @return string */
    public function getAttribute(string $name) {}

    /** @return string */
    public function getAttributeNS(?string $namespaceURI, string $localName) {}

    /** @return DOMAttr|DOMNamespaceNode|false */
    public function getAttributeNode(string $name) {}

    /** @return DOMAttr|DOMNamespaceNode|null */
    public function getAttributeNodeNS(?string $namespaceURI, string $localName) {}

    /** @return DOMNodeList */
    public function getElementsByTagName(string $name) {}

    /** @return DOMNodeList */
    public function getElementsByTagNameNS(string $namespaceURI, string $localName) {}

    /** @return bool */
    public function hasAttribute(string $name) {}

    /** @return bool */
    public function hasAttributeNS(?string $namespaceURI, string $localName) {}

    /** @return bool */
    public function removeAttribute(string $name) {}

    /** @return DOMAttr|false */
    public function removeAttributeNS(?string $namespaceURI, string $localName) {}

    /** @return DOMAttr|false */
    public function removeAttributeNode(string $name) {}

    /** @return DOMAttr|bool */
    public function setAttribute(string $name, string $value) {}

    /** @return null|false */
    public function setAttributeNS(?string $namespaceURI, string $localName, string $value) {}

    /** @return DOMAttr|null|false */
    public function setAttributeNode(DOMAttr $attr) {}

    /** @return DOMAttr|null|false */
    public function setAttributeNodeNS(DOMAttr $attr) {}

    /** @return void */
    public function setIdAttribute(string $name, bool $isId) {}

    /** @return void */
    public function setIdAttributeNS(string $namespaceURI, string $localName, bool $isId) {}

    /** @return void */
    public function setIdAttributeNode(DOMAttr $attr, bool $isId) {}

    public function remove(): void {}

    /** @var ...DOMNode|string $nodes */
    public function before(... $nodes): void {}

    /** @var ...DOMNode|string $nodes */
    public function after(...$nodes): void {}

    /** @var ...DOMNode|string $nodes */
    public function replaceWith(...$nodes): void {}

    /** @var ...DOMNode|string $nodes */
    public function append(...$nodes): void {}

    /** @var ...DOMNode|string $nodes */
    public function prepend(...$nodes): void {}
}

class DOMDocument implements DOMParentNode
{
    public function __construct(string $version = "1.0", string $encoding = "") {}

    /** @return DOMAttr|false */
    public function createAttribute(string $name) {}

    /** @return DOMAttr|false */
    public function createAttributeNS(?string $namespaceURI, string $qualifiedName) {}

    /** @return DOMCdataSection|false */
    public function createCDATASection(string $data) {}

    /** @return DOMComment|false */
    public function createComment(string $data) {}

    /** @return DOMDocumentFragment|false */
    public function createDocumentFragment() {}

    /** @return DOMElement|false */
    public function createElement(string $tagName, string $value = "") {}

    /** @return DOMElement|false */
    public function createElementNS(?string $namespaceURI, string $qualifiedName, string $value = "") {}

    /** @return DOMEntityReference|false */
    public function createEntityReference(string $name) {}

    /** @return DOMProcessingInstruction|false */
    public function createProcessingInstruction(string $target, string $data = "") {}

    /** @return DOMText|false */
    public function createTextNode(string $data) {}

    /** @return DOMElement|null */
    public function getElementById(string $elementId) {}

    /** @return DOMNodeList */
    public function getElementsByTagName(string $tagName) {}

    /** @return DOMNodeList */
    public function getElementsByTagNameNS(string $namespaceURI, string $localName) {}

    /** @return DOMNode|false */
    public function importNode(DOMNode $importedNode, bool $deep = false) {}

    /** @return DOMDocument|bool */
    public function load(string $filename, int $options = 0) {}

    /** @return DOMDocument|bool */
    public function loadXML(string $source, int $options = 0) {}

    /** @return void */
    public function normalizeDocument() {}

    /** @return bool */
    public function registerNodeClass(string $baseclass, ?string $extendedclass) {}

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
    public function adoptNode(DOMNode $source) {}

    /** @var ...DOMNode|string $nodes */
    public function append(...$nodes): void {}

    /** @var ...DOMNode|string $nodes */
    public function prepend(...$nodes): void {}
}

final class DOMException extends Exception
{
}

class DOMText
{
    public function __construct(string $value = "") {}

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
    /** @return DOMNode|null */
    public function getNamedItem(string $name) {}

    /** @return DOMNode|null */
    public function getNamedItemNS(?string $namespaceURI, string $localName) {}

    /** @return DOMNode|null */
    public function item(int $index) {}

    /** @return int|false */
    public function count() {}

    public function getIterator(): Iterator {}
}

class DOMEntity extends DOMNode
{
}

class DOMEntityReference
{
    public function __construct(string $name) {}
}

class DOMNotation extends DOMNode
{
}

class DOMProcessingInstruction
{
    public function __construct(string $name, string $value = "") {}
}

#ifdef LIBXML_XPATH_ENABLED
class DOMXPath
{
    public function __construct(DOMDocument $doc, bool $registerNodeNS = true) {}

    /** @return mixed */
    public function evaluate(string $expr, ?DOMNode $context = null, bool $registerNodeNS = true) {}

    /** @return mixed */
    public function query(string $expr, ?DOMNode $context = null, bool $registerNodeNS = true) {}

    /** @return bool */
    public function registerNamespace(string $prefix, string $namespaceURI) {}

    /**
     * @param string|array $restrict
     * @return bool|null
     */
    public function registerPhpFunctions($restrict = null) {}
}
#endif

function dom_import_simplexml(object $node): ?DOMElement {}
