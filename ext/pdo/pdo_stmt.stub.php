<?php

/** @generate-function-entries */

class PDOStatement implements IteratorAggregate
{
    /** @return bool */
    public function bindColumn(string|int $column, mixed &$param, int $type = 0, int $maxlen = 0, mixed $driverdata = null) {}

    /** @return bool */
    public function bindParam(string|int $parameter, mixed &$param, int $type = PDO::PARAM_STR, int $maxlen = 0, mixed $driverdata = null) {}

    /** @return bool */
    public function bindValue(string|int $parameter, mixed $value, int $type = PDO::PARAM_STR) {}

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
    public function fetch(int $fetch_style = PDO::FETCH_BOTH, int $cursor_orientation = PDO::FETCH_ORI_NEXT, int $cursor_offset = 0) {}

    /** @return array */
    public function fetchAll(int $fetch_style = PDO::FETCH_BOTH, mixed ...$fetch_mode_args) {}

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
