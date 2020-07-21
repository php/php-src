<?php

/** @generate-function-entries */

// These are extension methods for PDO. This is not a real class.
class PDO_SQLite_Ext {
    /** @return bool */
    public function sqliteCreateFunction(string $function_name, callable $callback, int $num_args = -1, int $flags = 0) {}

    /** @return bool */
    public function sqliteCreateAggregate(string $function_name, callable $step_func, callable $finalize_func, int $num_args = -1) {}

    /** @return bool */
    public function sqliteCreateCollation(string $name, callable $callback) {}
}
