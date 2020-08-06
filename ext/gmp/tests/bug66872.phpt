--TEST--
Bug #66872: Crash when passing string to gmp_testbit
--SKIPIF--
<?php if (!extension_loaded("gmp")) print "skip"; ?>
--FILE--
<?php

try {
    var_dump(gmp_testbit("abc", 1));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
gmp_testbit(): Argument #1 ($a) is not an integer string
