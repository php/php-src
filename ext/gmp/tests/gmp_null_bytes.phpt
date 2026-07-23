--TEST--
GMP rejects integer strings containing null bytes
--EXTENSIONS--
gmp
--FILE--
<?php

$tests = [
    'gmp_init' => fn() => gmp_init("123\0abc"),
    'gmp_init prefix' => fn() => gmp_init("0x10\0ff"),
    'gmp_add' => fn() => gmp_add("123\0abc", 1),
    'constructor' => fn() => new GMP("123\0abc"),
];

foreach ($tests as $label => $test) {
    try {
        $test();
    } catch (ValueError $e) {
        echo $label, ": ", $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
gmp_init: gmp_init(): Argument #1 ($num) is not an integer string
gmp_init prefix: gmp_init(): Argument #1 ($num) is not an integer string
gmp_add: gmp_add(): Argument #1 ($num1) is not an integer string
constructor: GMP::__construct(): Argument #1 ($num) is not an integer string
