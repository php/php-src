--TEST--
Variadic arguments must have compatible passing modes
--FILE--
<?php

interface DB {
    public function query($query, &...$params);
}

class MySQL implements DB {
    public function query($query, ...$params) { }
}

?>
--EXPECTF--
Fatal error: Declaration of MySQL::query() must be compatible with DB::query($query, &...$params) in %s on line %d
