<?php

/** @generate-function-entries */

class PDO
{
    public function __construct(string $dsn, ?string $username = null, ?string $password = null, ?array $options = null) {}

    /** @return bool */
    public function beginTransaction() {}

    /** @return bool */
    public function commit() {}

    /** @return string|null */
    public function errorCode() {}

    /** @return array */
    public function errorInfo() {}

    /** @return int|false */
    public function exec(string $statement) {}

    /** @return bool|int|string|array|null */
    public function getAttribute(int $attribute) {}

    /** @return array */
    public static function getAvailableDrivers() {}

    /** @return bool */
    public function inTransaction() {}

    /** @return string|false */
    public function lastInsertId(?string $name = null) {}

    /** @return PDOStatement|false */
    public function prepare(string $query, array $options = []) {}

    /** @return PDOStatement|false */
    public function query(string $query, ?int $fetchMode = null, mixed ...$fetchModeArgs) {}

    /** @return string|false */
    public function quote(string $string, int $type = PDO::PARAM_STR) {}

    /** @return bool */
    public function rollBack() {}

    /** @return bool */
    public function setAttribute(int $attribute, mixed $value) {}
}
