<?php

/** @generate-function-entries */

class Directory
{
    /**
     * @param resource|null $dir_handle
     * @return void
     * @alias closedir
     */
    public function close($dir_handle = null) {}

    /**
     * @param resource|null $dir_handle
     * @return void
     * @alias rewinddir
     */
    public function rewind($dir_handle = null) {}

    /**
     * @param resource|null $dir_handle
     * @return string|false
     * @alias readdir
     */
    public function read($dir_handle = null) {}
}
