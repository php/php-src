<?php

/** @generate-class-entries */

#ifdef HAVE_GLOB
#ifdef GLOB_BRACE
/**
 * @var int
 * @cvalue GLOB_BRACE
 */
const GLOB_BRACE = UNKNOWN;
#endif
#ifdef GLOB_ERR
/**
 * @var int
 * @cvalue GLOB_ERR
 */
const GLOB_ERR = UNKNOWN;
#endif
#ifdef GLOB_MARK
/**
 * @var int
 * @cvalue GLOB_MARK
 */
const GLOB_MARK = UNKNOWN;
#endif
#ifdef GLOB_NOCHECK
/**
 * @var int
 * @cvalue GLOB_NOCHECK
 */
const GLOB_NOCHECK = UNKNOWN;
#endif
#ifdef GLOB_NOESCAPE
/**
 * @var int
 * @cvalue GLOB_NOESCAPE
 */
const GLOB_NOESCAPE = UNKNOWN;
#endif
#ifdef GLOB_NOSORT
/**
 * @var int
 * @cvalue GLOB_NOSORT
 */
const GLOB_NOSORT = UNKNOWN;
#endif
#ifdef GLOB_ONLYDIR
/**
 * @var int
 * @cvalue GLOB_ONLYDIR
 */
const GLOB_ONLYDIR = UNKNOWN;
#endif
#ifdef GLOB_AVAILABLE_FLAGS
/**
 * @var int
 * @cvalue GLOB_AVAILABLE_FLAGS
 */
const GLOB_AVAILABLE_FLAGS = UNKNOWN;
#endif
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

class Directory
{
    public readonly string $path;

    /** @var resource */
    public readonly mixed $handle;

    /**
     * @tentative-return-type
     * @implementation-alias closedir
     */
    public function close(): void {}

    /**
     * @tentative-return-type
     * @implementation-alias rewinddir
     */
    public function rewind(): void {}

    /**
     * @tentative-return-type
     * @implementation-alias readdir
     */
    public function read(): string|false {}
}
