<?php

/** @generate-class-entries */

/** @not-serializable */
class CURLFile
{
    public string $name = "";
    public string $mime = "";
    public string $postname = "";

    public function __construct(string $filename, ?string $mime_type = null, ?string $posted_filename = null) {}

    /** @tentative-return-type */
    public function getFilename(): string {}

    /** @tentative-return-type */
    public function getMimeType(): string {}

    /** @tentative-return-type */
    public function getPostFilename(): string {}

    /** @tentative-return-type */
    public function setMimeType(string $mime_type): void {}

    /** @tentative-return-type */
    public function setPostFilename(string $posted_filename): void {}
}

class CURLStringFile
{
    public string $data;
    public string $postname;
    public string $mime;

    public function __construct(string $data, string $postname, string $mime = "application/octet-stream") {}
}
