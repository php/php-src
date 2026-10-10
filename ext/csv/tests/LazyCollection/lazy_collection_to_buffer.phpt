--TEST--
Check that a LazyLaxCollection created from a buffer generates the same output
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$string = "One,Two,Three\r\nFour,Five,Six\r\nSeven,Eight,Nine\r\n";

$lazyCollection = Csv\LazyLaxCollection::createFromBuffer($string);

var_dump($string === Csv\collection_to_buffer($lazyCollection));

?>
--EXPECT--
bool(true)
