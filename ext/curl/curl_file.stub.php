<?php

/** @generate-class-entries */

class CURLFile
{
    public string $name = "";
    public string $mime = "";
    public string $postname = "";

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

class CURLStringFile
{
    public string $data;
    public string $postname;
    public string $mime;

    public function __construct(string $data, string $postname, string $mime = "application/octet-stream") {}
}
