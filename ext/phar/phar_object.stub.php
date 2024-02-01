<?php

/** @generate-class-entries */

class PharException extends Exception
{
}

class Phar extends RecursiveDirectoryIterator implements Countable, ArrayAccess
{
    /**
     * @var int
     * @cvalue PHAR_ENT_COMPRESSED_BZ2
     */
    const BZ2 = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_ENT_COMPRESSED_GZ
     */
    const GZ = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_ENT_COMPRESSED_NONE
     */
    const NONE = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_FORMAT_PHAR
     */
    const PHAR = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_FORMAT_TAR
     */
    const TAR = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_FORMAT_ZIP
     */
    const ZIP = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_ENT_COMPRESSION_MASK
     */
    const COMPRESSED = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_MIME_PHP
     */
    const PHP = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_MIME_PHPS
     */
    const PHPS = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_SIG_MD5
     */
    const MD5 = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_SIG_OPENSSL
     */
    const OPENSSL = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_SIG_OPENSSL_SHA256
     */
    const OPENSSL_SHA256 = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_SIG_OPENSSL_SHA512
     */
    const OPENSSL_SHA512 = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_SIG_SHA1
     */
    const SHA1 = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_SIG_SHA256
     */
    const SHA256 = UNKNOWN;
    /**
     * @var int
     * @cvalue PHAR_SIG_SHA512
     */
    const SHA512 = UNKNOWN;

    public function __construct(string $filename, int $flags = FilesystemIterator::SKIP_DOTS|FilesystemIterator::UNIX_PATHS, ?string $alias = null) {}

    public function __destruct() {}

    /** @tentative-return-type */
    public function addEmptyDir(string $directory): void {}

    /** @tentative-return-type */
    public function addFile(string $filename, ?string $localName = null): void {}

    /** @tentative-return-type */
    public function addFromString(string $localName, string $contents): void {}

    /** @tentative-return-type */
    public function buildFromDirectory(string $directory, string $pattern = ""): array {}

    /** @tentative-return-type */
    public function buildFromIterator(Traversable $iterator, ?string $baseDirectory = null): array {}

    /** @tentative-return-type */
    public function compressFiles(int $compression): void {}

    /** @return bool */
    public function decompressFiles() {} // TODO make return type void

    /** @tentative-return-type */
    public function compress(int $compression, ?string $extension = null): ?Phar {}

    /** @tentative-return-type */
    public function decompress(?string $extension = null): ?Phar {}

    /** @tentative-return-type */
    public function convertToExecutable(?int $format = null, ?int $compression = null, ?string $extension = null): ?Phar {}

    /** @tentative-return-type */
    public function convertToData(?int $format = null, ?int $compression = null, ?string $extension = null): ?PharData {}

    /** @return bool */
    public function copy(string $from, string $to) {} // TODO make return type void

    /** @tentative-return-type */
    public function count(int $mode = COUNT_NORMAL): int {}

    /** @return bool */
    public function delete(string $localName) {} // TODO make return type void

    /** @return bool */
    public function delMetadata() {} // TODO make return type void

    /** @tentative-return-type */
    public function extractTo(string $directory, array|string|null $files = null, bool $overwrite = false): bool {}

    /** @tentative-return-type */
    public function getAlias(): ?string {}

    /** @tentative-return-type */
    public function getPath(): string {}

    /** @tentative-return-type */
    public function getMetadata(array $unserializeOptions = []): mixed {}

    /** @tentative-return-type */
    public function getModified(): bool {}

    /** @tentative-return-type */
    public function getSignature(): array|false {}

    /** @tentative-return-type */
    public function getStub(): string {}

    /** @tentative-return-type */
    public function getVersion(): string {}

    /** @tentative-return-type */
    public function hasMetadata(): bool {}

    /** @tentative-return-type */
    public function isBuffering(): bool {}

    /** @tentative-return-type */
    public function isCompressed(): int|false {}

    /** @tentative-return-type */
    public function isFileFormat(int $format): bool {}

    /** @tentative-return-type */
    public function isWritable(): bool {}

    /**
     * @param string $localName
     * @tentative-return-type
     */
    public function offsetExists($localName): bool {}

    /**
     * @param string $localName
     * @tentative-return-type
     */
    public function offsetGet($localName): SplFileInfo {}

    /**
     * @param string $localName
     * @param resource|string $value
     * @tentative-return-type
     */
    public function offsetSet($localName, $value): void {}

    /**
     * @param string $localName
     * @tentative-return-type
     */
    public function offsetUnset($localName): void {}

    /** @tentative-return-type */
    public function setAlias(string $alias): bool {}

    /** @tentative-return-type */
    public function setDefaultStub(?string $index = null, ?string $webIndex = null): bool {}

    /** @tentative-return-type */
    public function setMetadata(mixed $metadata): void {}

    /** @tentative-return-type */
    public function setSignatureAlgorithm(int $algo, ?string $privateKey = null): void {}

    /**
     * @param resource|string $stub
     * @return bool
     */
    public function setStub($stub, int $length = UNKNOWN) {} // TODO make return type void

    /** @tentative-return-type */
    public function startBuffering(): void {}

    /** @tentative-return-type */
    public function stopBuffering(): void {}

    final public static function apiVersion(): string {}

    final public static function canCompress(int $compression = 0): bool {}

    final public static function canWrite(): bool {}

    final public static function createDefaultStub(?string $index = null, ?string $webIndex = null): string {}

    final public static function getSupportedCompression(): array {}

    final public static function getSupportedSignatures(): array {}

    final public static function interceptFileFuncs(): void {}

    final public static function isValidPharFilename(string $filename, bool $executable = true): bool {}

    final public static function loadPhar(string $filename, ?string $alias = null): bool {}

    final public static function mapPhar(?string $alias = null, int $offset = 0): bool {}

    final public static function running(bool $returnPhar = true): string {}

    final public static function mount(string $pharPath, string $externalPath): void {}

    final public static function mungServer(array $variables): void {}

    final public static function unlinkArchive(string $filename): bool {} // TODO make return type void

    final public static function webPhar(
        ?string $alias = null, ?string $index = null, ?string $fileNotFoundScript = null,
        array $mimeTypes = [], ?callable $rewrite = null): void {}
}

class PharData extends RecursiveDirectoryIterator implements Countable, ArrayAccess
{
    /**
     * @implementation-alias Phar::__construct
     * @no-verify PharData constructor accepts extra $format argument
     */
    public function __construct(string $filename, int $flags = FilesystemIterator::SKIP_DOTS|FilesystemIterator::UNIX_PATHS, ?string $alias = null, int $format = 0) {}

    /** @implementation-alias Phar::__destruct */
    public function __destruct() {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::addEmptyDir
     */
    public function addEmptyDir(string $directory): void {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::addFile
     */
    public function addFile(string $filename, ?string $localName = null): void {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::addFromString
     */
    public function addFromString(string $localName, string $contents): void {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::buildFromDirectory
     */
    public function buildFromDirectory(string $directory, string $pattern = ""): array {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::buildFromIterator
     */
    public function buildFromIterator(Traversable $iterator, ?string $baseDirectory = null): array {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::compressFiles
     */
    public function compressFiles(int $compression): void {}

    /**
     * @return bool
     * @implementation-alias Phar::decompressFiles
     */
    public function decompressFiles() {} // TODO make return type void

    /**
     * @tentative-return-type
     * @implementation-alias Phar::compress
     * @no-verify
     */
    public function compress(int $compression, ?string $extension = null): ?PharData {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::decompress
     * @no-verify
     */
    public function decompress(?string $extension = null): ?PharData {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::convertToExecutable
     */
    public function convertToExecutable(?int $format = null, ?int $compression = null, ?string $extension = null): ?Phar {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::convertToData
     */
    public function convertToData(?int $format = null, ?int $compression = null, ?string $extension = null): ?PharData {}

    /**
     * @return bool
     * @implementation-alias Phar::copy
     */
    public function copy(string $from, string $to) {} // TODO make return type void

    /**
     * @tentative-return-type
     * @implementation-alias Phar::count
     */
    public function count(int $mode = COUNT_NORMAL): int {}

    /**
     * @return bool
     * @implementation-alias Phar::delete
     */
    public function delete(string $localName) {} // TODO make return type void

    /**
     * @return bool
     * @implementation-alias Phar::delMetadata
     */
    public function delMetadata() {} // TODO make return type void

    /**
     * @tentative-return-type
     * @implementation-alias Phar::extractTo
     */
    public function extractTo(string $directory, array|string|null $files = null, bool $overwrite = false): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::getAlias
     */
    public function getAlias(): ?string {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::getPath
     */
    public function getPath(): string {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::getMetadata
     */
    public function getMetadata(array $unserializeOptions = []): mixed {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::getModified
     */
    public function getModified(): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::getSignature
     */
    public function getSignature(): array|false {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::getStub
     */
    public function getStub(): string {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::getVersion
     */
    public function getVersion(): string {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::hasMetadata
     */
    public function hasMetadata(): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::isBuffering
     */
    public function isBuffering(): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::isCompressed
     */
    public function isCompressed(): int|false {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::isFileFormat
     */
    public function isFileFormat(int $format): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::isWritable
     */
    public function isWritable(): bool {}

    /**
     * @param string $localName
     * @tentative-return-type
     * @implementation-alias Phar::offsetExists
     */
    public function offsetExists($localName): bool {}

    /**
     * @param string $localName
     * @tentative-return-type
     * @implementation-alias Phar::offsetGet
     */
    public function offsetGet($localName): SplFileInfo {}

    /**
     * @param string $localName
     * @param resource|string $value
     * @tentative-return-type
     * @implementation-alias Phar::offsetSet
     */
    public function offsetSet($localName, $value): void {}

    /**
     * @param string $localName
     * @tentative-return-type
     * @implementation-alias Phar::offsetUnset
     */
    public function offsetUnset($localName): void {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::setAlias
     */
    public function setAlias(string $alias): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::setDefaultStub
     */
    public function setDefaultStub(?string $index = null, ?string $webIndex = null): bool {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::setMetadata
     */
    public function setMetadata(mixed $metadata): void {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::setSignatureAlgorithm
     */
    public function setSignatureAlgorithm(int $algo, ?string $privateKey = null): void {}

    /**
     * @param resource|string $stub
     * @return bool
     * @implementation-alias Phar::setStub
     */
    public function setStub($stub, int $length = UNKNOWN) {} // TODO make return type void

    /**
     * @tentative-return-type
     * @implementation-alias Phar::startBuffering
     */
    public function startBuffering(): void {}

    /**
     * @tentative-return-type
     * @implementation-alias Phar::stopBuffering
     */
    public function stopBuffering(): void {}

    /** @implementation-alias Phar::apiVersion */
    final public static function apiVersion(): string {}

    /** @implementation-alias Phar::canCompress */
    final public static function canCompress(int $compression = 0): bool {}

    /** @implementation-alias Phar::canWrite */
    final public static function canWrite(): bool {}

    /** @implementation-alias Phar::createDefaultStub */
    final public static function createDefaultStub(?string $index = null, ?string $webIndex = null): string {}

    /** @implementation-alias Phar::getSupportedCompression */
    final public static function getSupportedCompression(): array {}

    /** @implementation-alias Phar::getSupportedSignatures */
    final public static function getSupportedSignatures(): array {}

    /** @implementation-alias Phar::interceptFileFuncs */
    final public static function interceptFileFuncs(): void {}

    /** @implementation-alias Phar::isValidPharFilename */
    final public static function isValidPharFilename(string $filename, bool $executable = true): bool {}

    /** @implementation-alias Phar::loadPhar */
    final public static function loadPhar(string $filename, ?string $alias = null): bool {}

    /** @implementation-alias Phar::mapPhar */
    final public static function mapPhar(?string $alias = null, int $offset = 0): bool {}

    /** @implementation-alias Phar::running */
    final public static function running(bool $returnPhar = true): string {}

    /** @implementation-alias Phar::mount */
    final public static function mount(string $pharPath, string $externalPath): void {}

    /** @implementation-alias Phar::mungServer */
    final public static function mungServer(array $variables): void {}

    /** @implementation-alias Phar::unlinkArchive */
    final public static function unlinkArchive(string $filename): bool {} // TODO make return type void

    /** @implementation-alias Phar::webPhar */
    final public static function webPhar(
        ?string $alias = null, ?string $index = null, ?string $fileNotFoundScript = null,
        array $mimeTypes = [], ?callable $rewrite = null): void {}
}

class PharFileInfo extends SplFileInfo
{
    public function __construct(string $filename) {}

    public function __destruct() {}

    /** @tentative-return-type */
    public function chmod(int $perms): void {}

    /** @return bool */
    public function compress(int $compression) {} // TODO make return type void

    /** @return bool */
    public function decompress() {} // TODO make return type void

    /** @return bool */
    public function delMetadata() {} // TODO make return type void

    /** @tentative-return-type */
    public function getCompressedSize(): int {}

    /** @tentative-return-type */
    public function getCRC32(): int {}

    /** @tentative-return-type */
    public function getContent(): string {}

    /** @tentative-return-type */
    public function getMetadata(array $unserializeOptions = []): mixed {}

    /** @tentative-return-type */
    public function getPharFlags(): int {}

    /** @tentative-return-type */
    public function hasMetadata(): bool {}

    /** @tentative-return-type */
    public function isCompressed(?int $compression = null): bool {}

    /** @tentative-return-type */
    public function isCRCChecked(): bool {}

    /** @tentative-return-type */
    public function setMetadata(mixed $metadata): void {}
}
