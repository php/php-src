--TEST--
Test Csv\row_to_array() with BOM at start and enclosures
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

/* This is inspired by https://github.com/php/php-src/issues/8649 */

$fields = [
    'Hello',
    'Field has spaces',
    'Has delimiter, inside field',
    'Has field escape sequence " inside field',
    'Basic',
];

$string = "\xEF\xBB\xBF\"\",\"key\",\"key2\"\r\n";

try {
    var_dump(Csv\row_to_array($string));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
Enclosure sequence is used in a non escaped field
