<?php

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
