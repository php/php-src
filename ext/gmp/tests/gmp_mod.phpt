--TEST--
gmp_mod tests()
--EXTENSIONS--
gmp
--FILE--
<?php

try {
    var_dump(gmp_mod("",""));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
var_dump(gmp_mod(0,1));
var_dump(gmp_mod(0,-1));

try {
    var_dump(gmp_mod(-1,0));
} catch (\DivisionByZeroError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(gmp_mod(array(), array()));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

$a = gmp_init("-100000000");
$b = gmp_init("353467");

var_dump(gmp_mod($a, $b));

echo "Done\n";
?>
--EXPECT--
gmp_mod(): Argument #1 ($num1) is not an integer string
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}
object(GMP)#2 (1) {
  ["num"]=>
  string(1) "0"
}
Modulo by zero
gmp_mod(): Argument #1 ($num1) must be of type GMP|string|int, array given
object(GMP)#4 (1) {
  ["num"]=>
  string(5) "31161"
}
Done
