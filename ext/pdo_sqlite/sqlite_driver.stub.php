<?php

/** @generate-function-entries */

// These are extension methods for PDO. This is not a real class.
class PDO_SQLite_Ext {
    /** @return bool */
    public function sqliteCreateFunction(string $name, callable $callback, int $numArgs = -1, int $flags = 0) {}

    /** @return bool */
    public function sqliteCreateAggregate(string $name, callable $step, callable $finalize, int $numArgs = -1) {}

    /** @return bool */
    public function sqliteCreateCollation(string $name, callable $callback) {}
}
