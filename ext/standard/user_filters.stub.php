<?php

/** @generate-class-entries */

class php_user_filter
{
    public string $filtername = "";
    public mixed $params = "";
    /** @var resource|null */
    public $stream = null;

    /**
     * @param resource $in
     * @param resource $out
     * @param int $consumed
     * @tentative-return-type
     */
    public function filter($in, $out, &$consumed, bool $closing): int {}

    /** @tentative-return-type */
    public function onCreate(): bool {}

    /** @tentative-return-type */
    public function onClose(): void {}
}
