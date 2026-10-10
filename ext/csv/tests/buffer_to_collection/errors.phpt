--TEST--
Error conditions for Csv\buffer_to_collection()
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$string = "One,Two,Three\r\nFour,Five\r\nSeven,Eight,Nine\r\n";

try {
    var_dump(Csv\buffer_to_collection($string));
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
ValueError: Buffer row 2 contains 2 fields compared to 3 fields on previous rows
