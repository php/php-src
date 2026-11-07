--TEST--
Csv\LazyLaxCollection::createFromBuffer() with standard parameters
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
        "Two",
        'Three',
    ],
    [
        'Four',
        "Five",
        'Six',
    ],
    [
        'Seven',
        "Eight",
        'Nine',
    ],
];

$string = "One,Two,ThreeあFour,Five,SixあSeven,Eight,Nineあ";

$it = Csv\LazyLaxCollection::createFromBuffer($string, ',', '"', 'あ');

foreach ($it as $nb => $row) {
    var_dump($row === $collection[$nb]);
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
