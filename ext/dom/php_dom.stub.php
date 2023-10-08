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

    public function __sleep(): array {}

    public function __wakeup(): void {}

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

    /** @param DOMNode|string $nodes */
    public function append(...$nodes): void {}

    /** @param DOMNode|string $nodes */
    public function prepend(...$nodes): void {}
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
    public function appendData(string $data): bool {}

    /** @return string|false */
    public function substringData(int $offset, int $count) {}

    /** @tentative-return-type */
    public function insertData(int $offset, string $data): bool {}

    /** @tentative-return-type */
    public function deleteData(int $offset, int $count): bool {}

    /** @tentative-return-type */
    public function replaceData(int $offset, int $count, string $data): bool {}

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

    public string $value;

    /** @readonly */
    public ?DOMElement $ownerElement;

    /** @readonly */
    public mixed $schemaTypeInfo = null;

    public function __construct(string $name, string $value = "") {}

    /** @tentative-return-type */
    public function isId(): bool {}
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

    /** @tentative-return-type */
    public function getAttribute(string $qualifiedName): string {}

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

    /** @tentative-return-type */
    public function getElementsByTagName(string $qualifiedName): DOMNodeList {}

    /** @tentative-return-type */
    public function getElementsByTagNameNS(?string $namespace, string $localName): DOMNodeList {}

    /** @return DOMNode|false */
    public function importNode(DOMNode $node, bool $deep = false) {}

    /** @return DOMDocument|bool */
    public function load(string $filename, int $options = 0) {} // TODO return type shouldn't depend on the call scope

    /** @return DOMDocument|bool */
    public function loadXML(string $source, int $options = 0) {} // TODO return type shouldn't depend on the call scope

    /** @tentative-return-type */
    public function normalizeDocument(): void {}

    /** @tentative-return-type */
    public function registerNodeClass(string $baseClass, ?string $extendedClass): bool {}

    /** @tentative-return-type */
    public function save(string $filename, int $options = 0): int|false {}

#ifdef LIBXML_HTML_ENABLED
    /** @return DOMDocument|bool */
    public function loadHTML(string $source, int $options = 0) {} // TODO return type shouldn't depend on the call scope

    /** @return DOMDocument|bool */
    public function loadHTMLFile(string $filename, int $options = 0) {} // TODO return type shouldn't depend on the call scope

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

    /** @return DOMNode|false */
    public function adoptNode(DOMNode $node) {}

    /** @param DOMNode|string $nodes */
    public function append(...$nodes): void {}

    /** @param DOMNode|string $nodes */
    public function prepend(...$nodes): void {}
}

final class DOMException extends Exception
{
    /**
     * Intentionally left untyped for BC reasons
     * @var int
     */
    public $code = 0;
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
    public function getNamedItem(string $qualifiedName): ?DOMNode {} // TODO DOM spec returns DOMAttr

    /** @tentative-return-type */
    public function getNamedItemNS(?string $namespace, string $localName): ?DOMNode {} // TODO DOM spec returns DOMAttr

    /** @tentative-return-type */
    public function item(int $index): ?DOMNode {} // TODO DOM spec returns DOMAttr

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
}
#endif

function dom_import_simplexml(object $node): DOMElement {}
