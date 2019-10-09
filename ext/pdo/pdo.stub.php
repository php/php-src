<?php

/* pdo.c */

function pdo_drivers(): array {}

/* pdo_dbh.c */

class PDO {
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

/* pdo_stmt.c */

class PDOStatement implements Traversable {
    /**
     * @param mixed $driverdata
     * @return bool
     */
    public function bindColumn(int|string $column, &$param, int $type = 0, int $maxlen = 0, $driverdata = null) {}

    /**
     * @param mixed $driver_options
     * @return bool
     */
    public function bindParam(int|string $parameter, &$param, int $type = PDO::PARAM_STR, int $maxlen = 0, $driverdata = null) {}

    /**
     * @param int|string $parameter
     * @param mixed $value
     * @return bool
     */
    public function bindValue($parameter, $value, int $type = PDO::PARAM_STR) {}

    /** @return bool */
    public function closeCursor() {}

    /** @return int|false */
    public function columnCount() {}

    /** @return false|null */
    public function debugDumpParams() {}

    /** @return string|false|null */
    public function errorCode() {}

    /** @return array|false */
    public function errorInfo() {}

    /** @return bool */
    public function execute(?array $input_parameters = null) {}

    /** @return mixed */
    public function fetch(int $fetch_style = PDO::FETCH_BOTH, int $cursor_orientation = PDO::FETCH_ORI_NEXT, int $cursor_offset = 0) {}

    /**
     * @param mixed $fetch_argument
     * @return array|false
     */
    public function fetchAll(int $fetch_style = PDO::FETCH_BOTH, $fetch_argument = UNKNOWN, array $ctor_args = []) {}

    /** @return mixed */
    public function fetchColumn(int $column_number = 0) {}

    /** @return mixed */
    public function fetchObject(?string $class_name = "stdClass", ?array $ctor_args = null) {}

    /** @return mixed */
    public function getAttribute(int $attribute) {}

    /** @return array|false */
    public function getColumnMeta(int $column) {}

    /** @return bool */
    public function nextRowset() {}

    /** @return int|false */
    public function rowCount() {}

    /**
     * @param mixed $value
     * @return bool
     */
    public function setAttribute(int $attribute, $value) {}

    /** @return bool */
    public function setFetchMode(int $mode, ...$params) {}
}
