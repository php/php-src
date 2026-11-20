--TEST--
Csv\LazyLaxCollection::createFromBuffer() with custom EOL sequence as a nul byte
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

$string = "One,Two,Three\0Four,Five,Six\0Seven,Eight,Nine\0";

$it = Csv\LazyLaxCollection::createFromBuffer($string, ',', '"', "\0");

foreach ($it as $nb => $row) {
    var_dump($row === $collection[$nb]);
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
