<?php

class DOMNamedNodeMap
{
    /** @return ?DOMNode */
    public function getNamedItem(string $name) {}

    /** @return ?DOMNode */
    public function getNamedItemNS(?string $namespaceURI, string $localName) {}

    /** @return ?DOMNode */
    public function item(int $index) {}

    /** @return int|false */
    public function count() {}
}
