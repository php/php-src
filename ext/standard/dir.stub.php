<?php

class Directory
{
    /**
     * @param resource $dir_handle
     * @return void
     */
    public function close($dir_handle = UNKNOWN) {}

    /**
     * @param resource $dir_handle
     * @return void
     */
    public function rewind($dir_handle = UNKNOWN) {}

    /**
     * @param resource $dir_handle
     * @return string|false
     */
    public function read($dir_handle = UNKNOWN) {}
}
