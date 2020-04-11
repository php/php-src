<?php

class DOMDocument implements DOMParentNode
{
    public function __construct(string $version = "1.0", string $encoding = UNKNOWN) {}

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

#if defined(LIBXML_HTML_ENABLED)
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

#if defined(LIBXML_SCHEMAS_ENABLED)
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
