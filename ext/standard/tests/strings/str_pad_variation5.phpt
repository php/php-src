--TEST--
Test str_pad() function : usage variations - unexpected large value for '$pad_length' argument
--INI--
memory_limit=128M
--SKIPIF--
<?php
if (PHP_SYS_SIZE >= PHP_INT_SIZE) {
    die("skip this test is for PHP_SyS_SIZE < PHP_INT_SIZE only");
}
if (getenv("USE_ZEND_ALLOC") === "0") {
    die("skip Zend MM disabled");
}

--FILE--
<?php
$input = "Test string";

try {
    var_dump( str_pad($input, PHP_INT_MAX) );
} catch (ValueError $e) {
    echo $e::class . ": {$e->getMessage()}\n";
}

var_dump( str_pad($input, 2**(PHP_SYS_SIZE*8-1)-1) );

--EXPECTF--
ValueError: str_pad(): Argument #2 ($length) must be lower or equal to %d

Fatal error: Allowed memory size of %d bytes exhausted%s(tried to allocate %d bytes) in %s on line %d
