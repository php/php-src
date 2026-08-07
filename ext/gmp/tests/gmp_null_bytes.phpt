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
        echo $label, ': ', $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
gmp_init: ValueError: gmp_init(): Argument #1 ($num) is not an integer string
gmp_init prefix: ValueError: gmp_init(): Argument #1 ($num) is not an integer string
gmp_add: ValueError: gmp_add(): Argument #1 ($num1) is not an integer string
constructor: ValueError: GMP::__construct(): Argument #1 ($num) is not an integer string
