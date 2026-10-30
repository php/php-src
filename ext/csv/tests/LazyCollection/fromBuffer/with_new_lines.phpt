--TEST--
Csv\LazyLaxCollection::createFromBuffer() with new lines in buffer
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
        "I have a\n new line",
        'Three',
    ],
    [
        'Four',
        "I have a\r carriage return",
        'Six',
    ],
    [
        'Seven',
        "I have a carriage\r\n return new line",
        'Nine',
    ],
];

$string = "One,\"I have a\n new line\",Three\r\nFour,\"I have a\r carriage return\",Six\r\nSeven,\"I have a carriage\r\n return new line\",Nine\r\n";

$it = Csv\LazyLaxCollection::createFromBuffer($string);

foreach ($it as $nb => $row) {
    var_dump($row === $collection[$nb]);
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
