--TEST--
A non-variadic function can be turned into a variadic one
--FILE--
<?php

interface DB {
    public function query($query);
}

class MySQL implements DB {
    public function query($query, ...$params) { }
}

?>
===DONE===
--EXPECT--
===DONE===
