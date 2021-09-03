--TEST--
Bug #66872: Crash when passing string to gmp_testbit
--EXTENSIONS--
gmp
--FILE--
<?php

try {
    var_dump(gmp_testbit("abc", 1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
gmp_testbit(): Argument #1 ($num) is not an integer string
