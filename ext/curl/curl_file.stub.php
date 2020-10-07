<?php

/**
 * @generate-function-entries
 * @generate-class-entries
 */

class CURLFile
{
    /** @var string */
    public $name = "";
    /** @var string */
    public $mime = "";
    /** @var string */
    public $postname = "";

    public function __construct(string $filename, ?string $mime_type = null, ?string $posted_filename = null) {}

    /** @return string */
    public function getFilename() {}

    /** @return string */
    public function getMimeType() {}

    /** @return string */
    public function getPostFilename() {}

    /** @return void */
    public function setMimeType(string $mime_type) {}

    /** @return void */
    public function setPostFilename(string $posted_filename) {}
}
