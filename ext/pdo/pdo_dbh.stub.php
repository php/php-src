<?php

/** @generate-class-entries */

class PDO
{
    public function __construct(string $dsn, ?string $username = null, ?string $password = null, ?array $options = null) {}

    /** @tentative-return-type */
    public function beginTransaction(): bool {}

    /** @tentative-return-type */
    public function commit(): bool {}

    /** @tentative-return-type */
    public function errorCode(): ?string {}

    /** @tentative-return-type */
    public function errorInfo(): array {}

    /** @tentative-return-type */
    public function exec(string $statement): int|false {}

    /** @tentative-return-type */
    public function getAttribute(int $attribute): mixed {}

    /** @tentative-return-type */
    public static function getAvailableDrivers(): array {}

    /** @tentative-return-type */
    public function inTransaction(): bool {}

    /** @tentative-return-type */
    public function lastInsertId(?string $name = null): string|false {}

    /** @tentative-return-type */
    public function prepare(string $query, array $options = []): PDOStatement|false {}

    /** @tentative-return-type */
    public function query(string $query, ?int $fetchMode = null, mixed ...$fetchModeArgs): PDOStatement|false {}

    /** @tentative-return-type */
    public function quote(string $string, int $type = PDO::PARAM_STR): string|false {}

    /** @tentative-return-type */
    public function rollBack(): bool {}

    /**
     * @param array|int $value
     * @tentative-return-type
     */
    public function setAttribute(int $attribute, $value): bool {}
}
