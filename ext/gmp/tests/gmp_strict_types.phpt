--TEST--
GMP functions with strict_types=1
--EXTENSIONS--
gmp
--FILE--
<?php

declare(strict_types=1);

var_dump(gmp_abs(gmp_init(-1)));
var_dump(gmp_abs(-1));
var_dump(gmp_abs("-1"));
try {
    gmp_abs(1.0);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    gmp_abs(false);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    gmp_abs(true);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    gmp_abs(null);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
try {
    gmp_abs([]);
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "1"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "1"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "1"
}
gmp_abs(): Argument #1 ($num) must be of type GMP|string|int, float given
gmp_abs(): Argument #1 ($num) must be of type GMP|string|int, false given
gmp_abs(): Argument #1 ($num) must be of type GMP|string|int, true given
gmp_abs(): Argument #1 ($num) must be of type GMP|string|int, null given
gmp_abs(): Argument #1 ($num) must be of type GMP|string|int, array given
