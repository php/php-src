<?php

/** @generate-class-entries */

/** @not-serializable */
class PDOStatement implements IteratorAggregate
{
    public string $queryString;

    /** @tentative-return-type */
    public function bindColumn(string|int $column, mixed &$var, int $type = PDO::PARAM_STR, int $maxLength = 0, mixed $driverOptions = null): bool {}

    /** @tentative-return-type */
    public function bindParam(string|int $param, mixed &$var, int $type = PDO::PARAM_STR, int $maxLength = 0, mixed $driverOptions = null): bool {}

    /** @tentative-return-type */
    public function bindValue(string|int $param, mixed $value, int $type = PDO::PARAM_STR): bool {}

    /** @tentative-return-type */
    public function closeCursor(): bool {}

    /** @tentative-return-type */
    public function columnCount(): int {}

    /** @tentative-return-type */
    public function debugDumpParams(): ?bool {}

    /** @tentative-return-type */
    public function errorCode(): ?string {}

    /** @tentative-return-type */
    public function errorInfo(): array {}

    /** @tentative-return-type */
    public function execute(?array $params = null): bool {}

    /** @tentative-return-type */
    public function fetch(int $mode = PDO::FETCH_DEFAULT, int $cursorOrientation = PDO::FETCH_ORI_NEXT, int $cursorOffset = 0): mixed {}

    /** @tentative-return-type */
    public function fetchAll(int $mode = PDO::FETCH_DEFAULT, mixed ...$args): array {}

    /** @tentative-return-type */
    public function fetchColumn(int $column = 0): mixed {}

    /** @tentative-return-type */
    public function fetchObject(?string $class = "stdClass", array $constructorArgs = []): object|false {}

    /** @tentative-return-type */
    public function getAttribute(int $name): mixed {}

    /** @tentative-return-type */
    public function getColumnMeta(int $column): array|false {}

    /** @tentative-return-type */
    public function nextRowset(): bool {}

    /** @tentative-return-type */
    public function rowCount(): int {}

    /** @tentative-return-type */
    public function setAttribute(int $attribute, mixed $value): bool {}

    /** @return bool */
    public function setFetchMode(int $mode, mixed ...$args) {} // TODO make return type void

    public function getIterator(): Iterator {}
}

/** @not-serializable */
final class PDORow
{
    public string $queryString;
}
