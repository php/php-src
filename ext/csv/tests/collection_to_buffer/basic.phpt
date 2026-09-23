--TEST--
Test Csv\collection_to_buffer() with standard parameters
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$collection = [
    [
        'One',
        'Two',
        'Three',
    ],
    [
        'Four',
        'Five',
        'Six',
    ],
    [
        'Seven',
        'Eight',
        'Nine',
    ],
];

$output = "One,Two,Three\r\nFour,Five,Six\r\nSeven,Eight,Nine\r\n";

var_dump($output === Csv\collection_to_buffer($collection));
var_dump(Csv\collection_to_buffer($collection));
var_dump(Csv\collection_to_buffer($collection, ','));
var_dump(Csv\collection_to_buffer($collection, ',', '"'));
var_dump(Csv\collection_to_buffer($collection, ',', '"', "\r\n"));

?>
--EXPECT--
bool(true)
string(48) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
"
string(48) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
"
string(48) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
"
string(48) "One,Two,Three
Four,Five,Six
Seven,Eight,Nine
"
