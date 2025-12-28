<?php

/** @generate-class-entries */

namespace Pdo;

/**
 * @strict-properties
 * @not-serializable
 */
class Pgsql extends \PDO
{
    /** @cvalue PDO_PGSQL_ATTR_DISABLE_PREPARES */
    public const int ATTR_DISABLE_PREPARES = UNKNOWN;

#ifdef HAVE_PG_RESULT_MEMORY_SIZE
    /** @cvalue PDO_PGSQL_ATTR_RESULT_MEMORY_SIZE */
    public const int ATTR_RESULT_MEMORY_SIZE = UNKNOWN;
#endif

    /** @cvalue PGSQL_TRANSACTION_IDLE */
    #[\Deprecated(since: "8.5", message: "as it has no effect")]
    public const int TRANSACTION_IDLE = UNKNOWN;

    /** @cvalue PGSQL_TRANSACTION_ACTIVE */
    #[\Deprecated(since: "8.5", message: "as it has no effect")]
    public const int TRANSACTION_ACTIVE = UNKNOWN;

    /** @cvalue PGSQL_TRANSACTION_INTRANS */
    #[\Deprecated(since: "8.5", message: "as it has no effect")]
    public const int TRANSACTION_INTRANS = UNKNOWN;

    /**  @cvalue PGSQL_TRANSACTION_INERROR */
    #[\Deprecated(since: "8.5", message: "as it has no effect")]
    public const int TRANSACTION_INERROR = UNKNOWN;

    /** @cvalue PGSQL_TRANSACTION_UNKNOWN */
    #[\Deprecated(since: "8.5", message: "as it has no effect")]
    public const int TRANSACTION_UNKNOWN = UNKNOWN;

    public function escapeIdentifier(string $input): string {}

    public function copyFromArray(string $tableName, array $rows, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null): bool {}

    public function copyFromFile(string $tableName, string $filename, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null): bool {}

    public function copyToArray(string $tableName, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null): array|false {}

    public function copyToFile(string $tableName, string $filename, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null): bool {}

    public function lobCreate(): string|false {}

    /**
     * Opens an existing large object stream. Must be called inside a transaction.
     * @return resource|false
     */
    public function lobOpen(string $oid, string $mode = "rb") {}

    public function lobUnlink(string $oid): bool {}

    public function getNotify(int $fetchMode = \PDO::FETCH_DEFAULT, int $timeoutMilliseconds = 0): array|false {}

    public function getPid(): int {}

    public function setNoticeCallback(?callable $callback): void {}
}
