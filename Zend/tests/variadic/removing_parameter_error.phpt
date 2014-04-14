--TEST--
It's not possible to remove required parameter before a variadic parameter
--FILE--
<?php

/* Theoretically this should be valid because it weakens the constraint, but
 * PHP does not allow this (for non-variadics), so I'm not allowing it here, too,
 * to stay consistent. */

interface DB {
    public function query($query, ...$params);
}

class MySQL implements DB {
    public function query(...$params) { }
}

?>
--EXPECTF--
Fatal error: Declaration of MySQL::query() must be compatible with DB::query($query, ...$params) in %s on line %d
