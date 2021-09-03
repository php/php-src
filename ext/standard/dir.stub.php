<?php

/** @generate-class-entries */

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
