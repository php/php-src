--TEST--
Csv\buffer_to_collection() non escaped enclosure in one row
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$string = "One,Two,Three\r\nFour,De\"ux,Six\r\nSeven,Eight,Nine\r\n";
try {
    var_dump(Csv\buffer_to_collection($string));
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

$string = "One,Two,Three\r\nFour,Deあux,Six\r\nSeven,Eight,Nine\r\n";
try {
    var_dump(Csv\buffer_to_collection($string, ',', 'あ'));
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
ValueError: Enclosure sequence is used in a non escaped field
ValueError: Enclosure sequence is used in a non escaped field
