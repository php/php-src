<?php

/** @generate-function-entries */

// These are extension methods for PDO. This is not a real class.
class PDO_PGSql_Ext {
    /** @return bool */
    public function pgsqlCopyFromArray(string $table_name, array $rows, string $delimiter = "\t", string $null_as = "\\\\N", string $fields = UNKNOWN) {}

    /** @return bool */
    public function pgsqlCopyFromFile(string $table_name, string $filename, string $delimiter = "\t", string $null_as = "\\\\N", string $fields = UNKNOWN) {}

    /** @return array|false */
    public function pgsqlCopyToArray(string $table_name, string $delimiter = "\t", string $null_as = "\\\\N", string $fields = UNKNOWN) {}

    /** @return bool */
    public function pgsqlCopyToFile(string $table_name, string $filename, string $delimiter = "\t", string $null_as = "\\\\N", string $fields = UNKNOWN) {}

    /** @return string|false */
    public function pgsqlLOBCreate() {}

    /** @return resource|false */
    public function pgsqlLOBOpen(string $oid, string $mode = "rb") {}

    /** @return bool */
    public function pgsqlLOBUnlink(string $oid) {}

    /** @return array|false */
    public function pgsqlGetNotify(int $result_type = PDO::FETCH_USE_DEFAULT, int $ms_timeout = 0) {}

    /** @return int */
    public function pgsqlGetPid() {}
}
