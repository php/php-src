--TEST--
It's not possible to turn a variadic function into a non-variadic one
--FILE--
<?php

interface DB {
    public function query($query, ...$params);
}

class MySQL implements DB {
    public function query($query, $params) { }
}

?>
--EXPECTF--
Fatal error: Declaration of MySQL::query() must be compatible with DB::query($query, ...$params) in %s on line %d
