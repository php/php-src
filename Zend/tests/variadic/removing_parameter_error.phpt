--TEST--
It is possible to remove required parameter before a variadic parameter
--FILE--
<?php

interface DB {
    public function query($query, ...$params);
}

class MySQL implements DB {
    public function query(...$params) { }
}

?>
===DONE===
--EXPECT--
===DONE===
