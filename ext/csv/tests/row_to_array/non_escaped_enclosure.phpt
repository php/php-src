--TEST--
Test Csv\row_to_array() with enclosure in middle of field
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$string = "key1,ke\"y2,key3\r\n";

try {
    var_dump(Csv\row_to_array($string));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

$string = "key1,keあy2,key3\r\n";
try {
    var_dump(Csv\row_to_array($string, ',', 'あ'));
} catch (\ValueError $e) {
    echo $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
Enclosure sequence is used in a non escaped field
Enclosure sequence is used in a non escaped field
