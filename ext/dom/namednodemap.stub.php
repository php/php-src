<?php

class DOMNamedNodeMap
{
    /** @return DOMNode|null */
    public function getNamedItem(string $name) {}

    /** @return DOMNode|null */
    public function getNamedItemNS(?string $namespaceURI, string $localName) {}

    /** @return DOMNode|null */
    public function item(int $index) {}

    /** @return int|false */
    public function count() {}
}
