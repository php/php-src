--TEST--
Additional optional parameters must have a matching prototype
--FILE--
<?php

interface DB {
    public function query($query, string ...$params);
}

class MySQL implements DB {
    public function query($query, int $extraParam = null, string ...$params) { }
}

?>
--EXPECTF--
Fatal error: Declaration of MySQL::query($query, ?int $extraParam = NULL, string ...$params) must be compatible with DB::query($query, string ...$params) in %s on line %d
