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

$output = "One,Two,ThreeあFour,Five,SixあSeven,Eight,Nineあ";

var_dump($output === Csv\collection_to_buffer($collection, ',', '"', 'あ'));
var_dump(Csv\collection_to_buffer($collection, ',', '"', 'あ'));

?>
--EXPECT--
bool(true)
string(51) "One,Two,ThreeあFour,Five,SixあSeven,Eight,Nineあ"
