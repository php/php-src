<?php

/** @generate-class-entries */

#if (defined(HAVE_GLOB) && defined(GLOB_BRACE))
/**
 * @var int
 * @cvalue GLOB_BRACE
 */
const GLOB_BRACE = UNKNOWN;
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_ERR))
/**
 * @var int
 * @cvalue GLOB_ERR
 */
const GLOB_ERR = UNKNOWN;
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_MARK))
/**
 * @var int
 * @cvalue GLOB_MARK
 */
const GLOB_MARK = UNKNOWN;
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_NOCHECK))
/**
 * @var int
 * @cvalue GLOB_NOCHECK
 */
const GLOB_NOCHECK = UNKNOWN;
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_NOESCAPE))
/**
 * @var int
 * @cvalue GLOB_NOESCAPE
 */
const GLOB_NOESCAPE = UNKNOWN;
#endif
#if (defined(HAVE_GLOB) && defined(GLOB_NOSORT))
/**
 * @var int
 * @cvalue GLOB_NOSORT
 */
const GLOB_NOSORT = UNKNOWN;
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
