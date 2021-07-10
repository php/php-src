--TEST--
Testing boolval()
--EXTENSIONS--
gmp
--FILE--
<?php
try {
    var_dump(boolval(gmp_init('5')));
} catch (\TypeError $e) {
    echo $e->getMessage(), \PHP_EOL;
}
?>
--EXPECT--
Object of class GMP could not be converted to bool
