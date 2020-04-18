<?php

/** @generate-function-entries */

class PDO
{
    public function __construct(string $dsn, ?string $username = null, ?string $passwd = null, ?array $options = null) {}

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

    /** @return mixed */
    public function getAttribute(int $attribute) {}

    /** @return array */
    public static function getAvailableDrivers() {}

    /** @return bool */
    public function inTransaction() {}

    /** @return string|false */
    public function lastInsertId(?string $name = null) {}

    /** @return PDOStatement|false */
    public function prepare(string $statement, array $driver_options = []) {}

    /** @return PDOStatement|false */
    public function query(string $statement) {}

    /** @return string|false */
    public function quote(string $string, int $parameter_type = PDO::PARAM_STR) {}

    /** @return bool */
    public function rollBack() {}

    /**
     * @param mixed $value
     * @return bool
     */
    public function setAttribute(int $attribute, $value) {}
}
