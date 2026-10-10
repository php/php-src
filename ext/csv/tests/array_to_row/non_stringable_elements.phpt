--TEST--
Test Csv\array_to_row() with an array not containing string elements
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$fields = [
    'Hello',
    'Field has spaces',
    ['Not a string'],
    'Has field escape sequence " inside field',
    'Basic',
];

try {
    var_dump(Csv\array_to_row($fields));
} catch (\Throwable $e) {
    echo $e::class, $e->getMessage(), \PHP_EOL;
}

$fields = [
    'Hello',
    'Field has spaces',
    new stdClass(),
    'Has field escape sequence " inside field',
    'Basic',
];

try {
    var_dump(Csv\array_to_row($fields));
} catch (\Throwable $e) {
    echo $e::class, $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d
string(80) "Hello,Field has spaces,Array,"Has field escape sequence "" inside field",Basic
"
ErrorObject of class stdClass could not be converted to string
