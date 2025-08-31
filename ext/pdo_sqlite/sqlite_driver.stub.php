<?php

/** @generate-function-entries */

/**
 * These are extension methods for PDO. This is not a real class.
 * @undocumentable
 */
class PDO_SQLite_Ext {
    /** @tentative-return-type */
    public function sqliteCreateFunction(string $name, callable $callback, int $numArgs = -1, int $flags = 0): bool {}

    /** @tentative-return-type */
    public function sqliteCreateAggregate(string $name, callable $step, callable $finalize, int $numArgs = -1): bool {}

    /** @tentative-return-type */
    public function sqliteCreateCollation(string $name, callable $callback): bool {}
}
