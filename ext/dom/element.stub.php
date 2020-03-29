<?php

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
