<?php

/** @generate-function-entries */

class PDOStatement implements IteratorAggregate
{
    /** @return bool */
    public function bindColumn(string|int $column, mixed &$param, int $type = 0, int $max_length = 0, mixed $driver_options = null) {}

    /** @return bool */
    public function bindParam(string|int $param, mixed &$bind_var, int $type = PDO::PARAM_STR, int $max_length = 0, mixed $driver_options = null) {}

    /** @return bool */
    public function bindValue(string|int $param, mixed $value, int $type = PDO::PARAM_STR) {}

    /** @return bool */
    public function closeCursor() {}

    /** @return int */
    public function columnCount() {}

    /** @return bool|null */
    public function debugDumpParams() {}

    /** @return string|null */
    public function errorCode() {}

    /** @return array */
    public function errorInfo() {}

    /** @return bool */
    public function execute(?array $input_parameters = null) {}

    /** @return mixed */
    public function fetch(int $mode = PDO::FETCH_BOTH, int $cursor_orientation = PDO::FETCH_ORI_NEXT, int $cursor_offset = 0) {}

    /** @return array */
    public function fetchAll(int $mode = PDO::FETCH_BOTH, mixed ...$args) {}

    /** @return mixed */
    public function fetchColumn(int $index = 0) {}

    /** @return mixed */
    public function fetchObject(?string $class = "stdClass", ?array $constructor_args = null) {}

    /** @return mixed */
    public function getAttribute(int $name) {}

    /** @return array|false */
    public function getColumnMeta(int $index) {}

    /** @return bool */
    public function nextRowset() {}

    /** @return int */
    public function rowCount() {}

    /** @return bool */
    public function setAttribute(int $attribute, mixed $value) {}

    /** @return bool */
    public function setFetchMode(int $mode, mixed ...$fetch_mode_args) {}

    public function getIterator(): Iterator {}
}

final class PDORow
{
}
