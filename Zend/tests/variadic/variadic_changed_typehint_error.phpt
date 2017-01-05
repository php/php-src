--TEST--
Typehints for variadic arguments have to be compatible
--FILE--
<?php

interface DB {
    public function query($query, string ...$params);
}

class MySQL implements DB {
    public function query($query, int ...$params) { }
}

?>
--EXPECTF--
Fatal error: Declaration of MySQL::query($query, int ...$params) must be compatible with DB::query($query, string ...$params) in %s on line %d
