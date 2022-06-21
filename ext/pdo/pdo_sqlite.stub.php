<?php

/** @generate-class-entries */

/** @not-serializable */
class PDOSqlite extends PDO
{
    public function createFunction(
        string $function_name,
        callable $callback,
        int $num_args = -1,
        int $flags = 0
    ): bool {}

// Whether SQLITE_OMIT_LOAD_EXTENSION is defined or not depends on how
// SQLite was compiled: https://www.sqlite.org/compile.html

#ifndef SQLITE_OMIT_LOAD_EXTENSION
    public function loadExtension(string $name): bool {}
#endif

}
