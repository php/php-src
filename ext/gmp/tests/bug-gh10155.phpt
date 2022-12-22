--TEST--
GMP constructor should not be called directly
--EXTENSIONS--
gmp
--FILE--
<?php
try {
    var_dump(new GMP(6));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Cannot directly construct GMP, use gmp_init() instead
