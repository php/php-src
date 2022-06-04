<?php

/** @generate-class-entries */

/** @not-serializable */
class PDOSqlite extends PDO
{
//    public function __construct(string $dsn, ?string $username = null, ?string $password = null, ?array $options = null) {}


    public function createFunction(
        string $function_name,
        callable $callback,
        int $num_args = -1,
        int $flags = 0
    ): bool {}
}
