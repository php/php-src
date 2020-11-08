<?php

/** @generate-function-entries */

// These are extension methods for PDO. This is not a real class.
class PDO_PGSql_Ext {
    /** @return bool */
    public function pgsqlCopyFromArray(string $tableName, array $rows, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null) {}

    /** @return bool */
    public function pgsqlCopyFromFile(string $tableName, string $filename, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null) {}

    /** @return array|false */
    public function pgsqlCopyToArray(string $tableName, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null) {}

    /** @return bool */
    public function pgsqlCopyToFile(string $tableName, string $filename, string $separator = "\t", string $nullAs = "\\\\N", ?string $fields = null) {}

    /** @return string|false */
    public function pgsqlLOBCreate() {}

    /** @return resource|false */
    public function pgsqlLOBOpen(string $oid, string $mode = "rb") {}

    /** @return bool */
    public function pgsqlLOBUnlink(string $oid) {}

    /** @return array|false */
    public function pgsqlGetNotify(int $fetchMode = PDO::FETCH_USE_DEFAULT, int $timeoutMilliseconds = 0) {}

    /** @return int */
    public function pgsqlGetPid() {}
}
