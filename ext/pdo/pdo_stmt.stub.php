<?php

/** @generate-class-entries */

class PDOStatement implements IteratorAggregate
{
    /** @var string|null */
    public $queryString;

    /** @return bool */
    public function bindColumn(string|int $column, mixed &$var, int $type = 0, int $maxLength = 0, mixed $driverOptions = null) {}

    /** @return bool */
    public function bindParam(string|int $param, mixed &$var, int $type = PDO::PARAM_STR, int $maxLength = 0, mixed $driverOptions = null) {}

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
    public function execute(?array $params = null) {}

    /** @return mixed */
    public function fetch(int $mode = PDO::FETCH_BOTH, int $cursorOrientation = PDO::FETCH_ORI_NEXT, int $cursorOffset = 0) {}

    /** @return array */
    public function fetchAll(int $mode = PDO::FETCH_BOTH, mixed ...$args) {}

    /** @return mixed */
    public function fetchColumn(int $column = 0) {}

    /** @return object|false */
    public function fetchObject(?string $class = "stdClass", ?array $ctorArgs = null) {}

    /** @return mixed */
    public function getAttribute(int $name) {}

    /** @return array|false */
    public function getColumnMeta(int $column) {}

    /** @return bool */
    public function nextRowset() {}

    /** @return int */
    public function rowCount() {}

    /** @return bool */
    public function setAttribute(int $attribute, mixed $value) {}

    /** @return bool */
    public function setFetchMode(int $mode, mixed ...$args) {}

    public function getIterator(): Iterator {}
}

final class PDORow
{
}
