<?php

/** @generate-class-entries */

/** @not-serializable */
class PdoPgsql extends PDO
{
    /**
     * @var int
     * @cname PDO_PGSQL_ATTR_DISABLE_PREPARES
     */
    public const ATTR_DISABLE_PREPARES = UNKNOWN;

    /**
     * @var int
     * @cname PGSQL_TRANSACTION_IDLE
     */
    public const TRANSACTION_IDLE = UNKNOWN;

    /**
     * @var int
     * @cname PGSQL_TRANSACTION_ACTIVE
     */
    public const TRANSACTION_ACTIVE = UNKNOWN;

    /**
     * @var int
     * @cname PGSQL_TRANSACTION_INTRANS
     */
    public const TRANSACTION_INTRANS = UNKNOWN;

    /**
     * @var int
     * @cname PGSQL_TRANSACTION_INERROR
     */
    public const TRANSACTION_INERROR = UNKNOWN;

    /**
     * @var int
     * @cname PGSQL_TRANSACTION_UNKNOWN
     */
    public const TRANSACTION_UNKNOWN = UNKNOWN;

    public function escapeIdentifier(string $input): string {}

    public function copyFromArray(string $tableName, array $rows, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null): bool {}

    public function copyFromFile(string $tableName, string $filename, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null): bool {}

    public function copyToArray(string $tableName, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null): array|false {}

    public function copyToFile(string $tableName, string $filename, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null): bool {}

    public function lobCreate(): string|false {}

    // Opens an existing large object stream.  Must be called inside a transaction.
    /** @return resource|false */
    public function lobOpen(string $oid, string $mode = "rb"){}

    public function lobUnlink(string $oid): bool {}

    public function getNotify(int $fetchMode = PDO::FETCH_USE_DEFAULT, int $timeoutMilliseconds = 0): array|false {}

    public function getPid(): int {}
}
