<?php

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
