<?php

/** @generate-class-entries */

/**
 * @return resource|int|false
 * @deprecated
 */
function zip_open(string $filename) {}

/**
 * @param resource $zip
 * @deprecated
 */
function zip_close($zip): void {}

/**
 * @param resource $zip
 * @return resource|false
 * @deprecated
 */
function zip_read($zip) {}

/**
 * @param resource $zip_dp
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_open($zip_dp, $zip_entry, string $mode = "rb"): bool {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_close($zip_entry): bool {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_read($zip_entry, int $len = 1024): string|false {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_name($zip_entry): string|false {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_compressedsize($zip_entry): int|false {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_filesize($zip_entry): int|false {}

/**
 * @param resource $zip_entry
 * @deprecated
 */
function zip_entry_compressionmethod($zip_entry): string|false {}

class ZipArchive implements Countable
{
    /** @readonly */
    public int $lastId;
    /** @readonly */
    public int $status;
    /** @readonly */
    public int $statusSys;
    /** @readonly */
    public int $numFiles;
    /** @readonly */
    public string $filename;
    /** @readonly */
    public string $comment;

    /** @tentative-return-type */
    public function open(string $filename, int $flags = 0): bool|int {}

    /** @tentative-return-type */
    public function setPassword(string $password): bool {}

    /** @tentative-return-type */
    public function close(): bool {}

    /** @tentative-return-type */
    public function count(): int {}

    /** @tentative-return-type */
    public function getStatusString(): string {}

    /** @tentative-return-type */
    public function addEmptyDir(string $dirname, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function addFromString(string $name, string $content, int $flags = ZipArchive::FL_OVERWRITE): bool {}

    /** @tentative-return-type */
    public function addFile(string $filepath, string $entryname = "", int $start = 0, int $length = 0, int $flags = ZipArchive::FL_OVERWRITE): bool {}

    /** @tentative-return-type */
    public function replaceFile(string $filepath, string $index, int $start = 0, int $length = 0, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function addGlob(string $pattern, int $flags = 0, array $options = []): array|false {}

    /** @tentative-return-type */
    public function addPattern(string $pattern, string $path = ".", array $options = []): array|false {}

    /** @tentative-return-type */
    public function renameIndex(int $index, string $new_name): bool {}

    /** @tentative-return-type */
    public function renameName(string $name, string $new_name): bool {}

    /** @tentative-return-type */
    public function setArchiveComment(string $comment): bool {}

    /** @tentative-return-type */
    public function getArchiveComment(int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function setCommentIndex(int $index, string $comment): bool {}

    /** @tentative-return-type */
    public function setCommentName(string $name, string $comment): bool {}

#ifdef HAVE_SET_MTIME
    /** @tentative-return-type */
    public function setMtimeIndex(int $index, int $timestamp, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function setMtimeName(string $name, int $timestamp, int $flags = 0): bool {}
#endif

    /** @tentative-return-type */
    public function getCommentIndex(int $index, int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function getCommentName(string $name, int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function deleteIndex(int $index): bool {}

    /** @tentative-return-type */
    public function deleteName(string $name): bool {}

    /** @tentative-return-type */
    public function statName(string $name, int $flags = 0): array|false {}

    /** @tentative-return-type */
    public function statIndex(int $index, int $flags = 0): array|false {}

    /** @tentative-return-type */
    public function locateName(string $name, int $flags = 0): int|false {}

    /** @tentative-return-type */
    public function getNameIndex(int $index, int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function unchangeArchive(): bool {}

    /** @tentative-return-type */
    public function unchangeAll(): bool {}

    /** @tentative-return-type */
    public function unchangeIndex(int $index): bool {}

    /** @tentative-return-type */
    public function unchangeName(string $name): bool {}

    /** @tentative-return-type */
    public function extractTo(string $pathto, array|string|null $files = null): bool {}

    /** @tentative-return-type */
    public function getFromName(string $name, int $len = 0, int $flags = 0): string|false {}

    /** @tentative-return-type */
    public function getFromIndex(int $index, int $len = 0, int $flags = 0): string|false {}

    /** @return resource|false */
    public function getStream(string $name) {}

#ifdef ZIP_OPSYS_DEFAULT
    /** @tentative-return-type */
    public function setExternalAttributesName(string $name, int $opsys, int $attr, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function setExternalAttributesIndex(int $index, int $opsys, int $attr, int $flags = 0): bool {}

    /**
     * @param int $opsys
     * @param int $attr
     * @tentative-return-type
     */
    public function getExternalAttributesName(string $name, &$opsys, &$attr, int $flags = 0): bool {}

    /**
     * @param int $opsys
     * @param int $attr
     * @tentative-return-type
     */
    public function getExternalAttributesIndex(int $index, &$opsys, &$attr, int $flags = 0): bool {}
#endif

    /** @tentative-return-type */
    public function setCompressionName(string $name, int $method, int $compflags = 0): bool {}

    /** @tentative-return-type */
    public function setCompressionIndex(int $index, int $method, int $compflags = 0): bool {}

#ifdef HAVE_ENCRYPTION
    /** @tentative-return-type */
    public function setEncryptionName(string $name, int $method, ?string $password = null): bool {}

    /** @tentative-return-type */
    public function setEncryptionIndex(int $index, int $method, ?string $password = null): bool {}
#endif

#ifdef HAVE_PROGRESS_CALLBACK
    /** @tentative-return-type */
    public function registerProgressCallback(float $rate, callable $callback): bool {}
#endif

#ifdef HAVE_CANCEL_CALLBACK
    /** @tentative-return-type */
    public function registerCancelCallback(callable $callback): bool {}
#endif

#ifdef HAVE_METHOD_SUPPORTED
    public static function isCompressionMethodSupported(int $method, bool $enc = true): bool {}

    public static function isEncryptionMethodSupported(int $method, bool $enc = true): bool {}
#endif
}
