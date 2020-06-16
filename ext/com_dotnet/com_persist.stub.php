<?php


final class COMPersistHelper
{
    public function __construct(?VARIANT $com_object) {}

    public function GetCurFileName(): string|false {}

    public function SaveToFile(?string $filename, bool $remember = true): bool {}

    public function LoadFromFile(string $path, int $flags = 0): bool {}

    public function GetMaxStreamSize(): int {}

    public function InitNew(): bool {}

    /** @param resource $stream */
    public function LoadFromStream($stream): bool {}

    /** @param resource $stream */
    public function SaveToStream($stream): bool {}
}
