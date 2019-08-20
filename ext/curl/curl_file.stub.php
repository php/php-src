<?php

class CURLFile {
    public function __construct(
        string $filename,
        string $mimetype = UNKNOWN,
        string $postname = UNKNOWN
    ) {}

    public function getFilename(): string {}

    public function getMimeType(): string {}

    public function getPostFilename(): string {}

    public function setMimeType(string $mime): void {}

    public function setPostFilename(string $postname): void {}
}
