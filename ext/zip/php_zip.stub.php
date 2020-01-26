<?php

/** @return resource|int|false */
function zip_open(string $filename) {}

/**
 * @param resource $zip
 */
function zip_close($zip): void {}

/**
 * @param resource $zip
 * @return resource|false
 */
function zip_read($zip) {}

/**
 * @param resource $zip_dp
 * @param resource $zip_entry
 */
function zip_entry_open($zip_dp, $zip_entry, string $mode = 'rb'): bool {}

/**
 * @param resource $zip_ent
 */
function zip_entry_close($zip_ent): bool {}

/** @param resource $zip_entry */
function zip_entry_read($zip_entry, int $len = 1024): string|false {}

/** @param resource $zip_entry */
function zip_entry_name($zip_entry): string|false {}

/** @param resource $zip_entry */
function zip_entry_compressedsize($zip_entry): int|false {}

/** @param resource $zip_entry */
function zip_entry_filesize($zip_entry): int|false {}

/** @param resource $zip_entry */
function zip_entry_compressionmethod($zip_entry): string|false {}

class ZipArchive
{
    /** @return bool|int */
    public function open(string $filename, int $flags = 0) {}

    /** @return bool */
    public function setPassword(string $password) {}

    /** @return bool */
    public function close() {}

    /** @return int */
    public function count() {}

    /** @return string */
    public function getStatusString() {}

    /** @return bool */
    public function addEmptyDir(string $dirname) {}

    /** @return bool */
    public function addFromString(string $name, string $content) {}

    /** @return bool */
    public function addFile(string $filepath, string $entryname = UNKNOWN, int $start = 0, int $length = 0) {}

    /** @return array|false */
    public function addGlob(string $pattern, int $flags = 0, $options = UNKNOWN) {}

    /** @return array|false */
    public function addPattern(string $pattern, string $path = UNKNOWN, $options = UNKNOWN) {}

    /** @return bool */
    public function renameIndex(int $index, string $new_name) {}

    /** @return bool */
    public function renameName(string $name, string $new_name) {}

    /** @return bool */
    public function setArchiveComment(string $comment) {}

    /** @return string|false */
    public function getArchiveComment(int $flags = 0) {}

    /** @return null|false */
    public function setCommentIndex(int $index, string $comment) {}

    /** @return null|false */
    public function setCommentName(string $name, string $comment) {}

    /** @return string|false */
    public function getCommentIndex(int $index, int $flags = 0) {}

    /** @return string|false */
    public function getCommentName(string $name, int $flags = 0) {}

    /** @return bool */
    public function deleteIndex(int $index) {}

    /** @return bool */
    public function deleteName(string $name) {}

    /** @return array|false */
    public function statName(string $filename, int $flags = 0) {}

    /** @return array|false */
    public function statIndex(int $index, int $flags = 0) {}

    /** @return int|false */
    public function locateName(string $filename, int $flags = 0) {}

    /** @return string|false */
    public function getNameIndex(int $index, int $flags = 0) {}

    /** @return bool */
    public function unchangeArchive() {}

    /** @return bool */
    public function unchangeAll() {}

    /** @return bool */
    public function unchangeIndex(int $index) {}

    /** @return bool */
    public function unchangeName(string $name) {}

    /** @return bool */
    public function extractTo(string $pathto, $files = UNKNOWN) {}

    /** @return string|false */
    public function getFromName(string $entryname, int $len = 0, int $flags = 0) {}

    /** @return string|false */
    public function getFromIndex(int $index, int $len = 0, int $flags = 0) {}

    /** @return resource|false */
    public function getStream(string $entryname) {}

#ifdef ZIP_OPSYS_DEFAULT
    /** @return bool */
    public function setExternalAttributesName(string $name, int $opsys, int $attr, int $flags = 0) {}

    /** @return bool */
    public function setExternalAttributesIndex(int $index, int $opsys, int $attr, int $flags = 0) {}

    /** @return bool */
    public function getExternalAttributesName(string $name, &$opsys, &$attr, int $flags = 0) {}

    /** @return bool */
    public function getExternalAttributesIndex(int $index, &$opsys, &$attr, int $flags = 0) {}
#endif

    /** @return bool */
    public function setCompressionName(string $name, int $method, int $compflags = 0) {}

    /** @return bool */
    public function setCompressionIndex(int $index, int $method, int $compflags = 0) {}

#ifdef HAVE_ENCRYPTION
    /** @return bool */
    public function setEncryptionName(string $name, int $method, string $password = UNKNOWN) {}

    /** @return bool */
    public function setEncryptionIndex(int $index, int $method, string $password = UNKNOWN) {}
#endif
}
