<?php

/** @generate-class-entries */

/**
 * @var int
 * @cvalue PSFS_PASS_ON
 */
const PSFS_PASS_ON = UNKNOWN;
/**
 * @var int
 * @cvalue PSFS_FEED_ME
 */
const PSFS_FEED_ME = UNKNOWN;
/**
 * @var int
 * @cvalue PSFS_ERR_FATAL
 */
const PSFS_ERR_FATAL = UNKNOWN;

/**
 * @var int
 * @cvalue PSFS_FLAG_NORMAL
 */
const PSFS_FLAG_NORMAL = UNKNOWN;
/**
 * @var int
 * @cvalue PSFS_FLAG_FLUSH_INC
 */
const PSFS_FLAG_FLUSH_INC = UNKNOWN;
/**
 * @var int
 * @cvalue PSFS_FLAG_FLUSH_CLOSE
 */
const PSFS_FLAG_FLUSH_CLOSE = UNKNOWN;

class php_user_filter
{
    public string $filtername = "";
    public mixed $params = "";
    /** @var resource|null */
    public $stream = null;

    /**
     * @param resource $in
     * @param resource $out
     * @tentative-return-type
     */
    public function filter($in, $out, int &$consumed, bool $closing): int {}

    /** @tentative-return-type */
    public function onCreate(): bool {}

    /** @tentative-return-type */
    public function onClose(): void {}
}
