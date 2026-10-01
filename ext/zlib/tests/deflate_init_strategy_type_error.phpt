--TEST--
deflate_init(): options type validation
--EXTENSIONS--
zlib
--FILE--
<?php

class A {}
$fp = fopen('php://memory', 'r');

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ['level' => 'foo']);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ['memory' => []]);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ['window' => new A()]);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    deflate_init(ZLIB_ENCODING_DEFLATE, ['strategy' => $fp]);
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

fclose($fp);

?>
--EXPECT--
TypeError: deflate_init(): Argument #2 ($options) the value for option "level" must be of type int, string given
TypeError: deflate_init(): Argument #2 ($options) the value for option "memory" must be of type int, array given
TypeError: deflate_init(): Argument #2 ($options) the value for option "window" must be of type int, A given
TypeError: deflate_init(): Argument #2 ($options) the value for option "strategy" must be of type int, resource given
