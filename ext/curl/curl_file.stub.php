<?php

class CURLFile {
    public function __construct(
        string $filename,
        string $mimetype = UNKNOWN,
        string $postname = UNKNOWN
    ) {}

    /** @return string */
    public function getFilename() {}

    /** @return string */
    public function getMimeType() {}

    /** @return string */
    public function getPostFilename() {}

    /** @return void */
    public function setMimeType(string $mime) {}

    /** @return void */
    public function setPostFilename(string $postname) {}
}
