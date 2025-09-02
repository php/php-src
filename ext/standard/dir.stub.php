<?php

/** @generate-class-entries */

/**
 * @var string
 * @cvalue dirsep_str
 */
const DIRECTORY_SEPARATOR = UNKNOWN;
/**
 * @var string
 * @cvalue pathsep_str
 */
const PATH_SEPARATOR = UNKNOWN;

#if (defined(PHP_GLOB_BRACE) && PHP_GLOB_BRACE != 0)
/**
 * @var int
 * @cvalue PHP_GLOB_BRACE
 */
const GLOB_BRACE = UNKNOWN;
#endif
#if (defined(PHP_GLOB_ERR) && PHP_GLOB_ERR != 0)
/**
 * @var int
 * @cvalue PHP_GLOB_ERR
 */
const GLOB_ERR = UNKNOWN;
#endif
#if (defined(PHP_GLOB_MARK) && PHP_GLOB_MARK != 0)
/**
 * @var int
 * @cvalue PHP_GLOB_MARK
 */
const GLOB_MARK = UNKNOWN;
#endif
#if (defined(PHP_GLOB_NOCHECK) && PHP_GLOB_NOCHECK != 0)
/**
 * @var int
 * @cvalue PHP_GLOB_NOCHECK
 */
const GLOB_NOCHECK = UNKNOWN;
#endif
#if (defined(PHP_GLOB_NOESCAPE) && PHP_GLOB_NOESCAPE != 0)
/**
 * @var int
 * @cvalue PHP_GLOB_NOESCAPE
 */
const GLOB_NOESCAPE = UNKNOWN;
#endif
#if (defined(PHP_GLOB_NOSORT) && PHP_GLOB_NOSORT != 0)
/**
 * @var int
 * @cvalue PHP_GLOB_NOSORT
 */
const GLOB_NOSORT = UNKNOWN;
#endif
#if (defined(PHP_GLOB_ONLYDIR) && PHP_GLOB_ONLYDIR != 0)
/**
 * @var int
 * @cvalue PHP_GLOB_ONLYDIR
 */
const GLOB_ONLYDIR = UNKNOWN;
#endif
#ifdef PHP_GLOB_AVAILABLE_FLAGS
/**
 * @var int
 * @cvalue PHP_GLOB_AVAILABLE_FLAGS
 */
const GLOB_AVAILABLE_FLAGS = UNKNOWN;
#endif
/**
 * @var int
 * @cvalue PHP_SCANDIR_SORT_ASCENDING
 */
const SCANDIR_SORT_ASCENDING = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_SCANDIR_SORT_DESCENDING
 */
const SCANDIR_SORT_DESCENDING = UNKNOWN;
/**
 * @var int
 * @cvalue PHP_SCANDIR_SORT_NONE
 */
const SCANDIR_SORT_NONE = UNKNOWN;

/**
 * @strict-properties
 * @not-serializable
 */
final class Directory
{
    public readonly string $path;

    /** @var resource */
    public readonly mixed $handle;

    public function close(): void {}

    public function rewind(): void {}

    public function read(): string|false {}
}
