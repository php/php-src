<?php

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

    /** @return ?string */
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

    /** @return ?string */
    public function lookupNamespaceUri(?string $prefix) {}

    /** @return ?string */
    public function lookupPrefix(string $namespaceURI) {}

    /** @return void */
    public function normalize() {}

    /** @return DOMNode|false */
    public function removeChild(DOMNode $oldChild) {}

    /** @return DOMNode|false */
    public function replaceChild(DOMNode $newChild, DOMNode $oldChild) {}
}
