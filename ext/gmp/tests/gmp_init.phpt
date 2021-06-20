--TEST--
gmp_init() basic tests
--EXTENSIONS--
gmp
--FILE--
<?php

var_dump(gmp_init("98765678"));
var_dump(gmp_strval(gmp_init("98765678")));
try {
    var_dump(gmp_init(1,-1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(gmp_init("",36));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_init("foo",3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(gmp_strval(gmp_init("993247326237679187178",3)));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

echo "Done\n";
?>
--EXPECT--
object(GMP)#1 (1) {
  ["num"]=>
  string(8) "98765678"
}
string(8) "98765678"
gmp_init(): Argument #2 ($base) must be between 2 and 62
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
gmp_init(): Argument #1 ($num) is not an integer string
Done
