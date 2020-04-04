<?php

class DOMText {
    public function __construct(string $value = "") {}

    /** @return bool */
    public function isWhitespaceInElementContent() {}

    /** @return bool */
    public function isElementContentWhitespace() {}

    /** @return DOMText|false */
    public function splitText(int $offset) {}
}
