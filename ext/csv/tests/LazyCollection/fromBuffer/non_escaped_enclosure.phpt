--TEST--
Csv\LazyLaxCollection::createFromBuffer() non escaped enclosure in one row
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$string = "One,Two,Three\r\nFour,De\"ux,Six\r\nSeven,Eight,Nine\r\n";

$it = Csv\LazyLaxCollection::createFromBuffer($string);

try {
    foreach ($it as $nb => $row) {
        var_dump($nb, $row);
    }
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

$string = "One,Two,Three\r\nFour,Deあux,Six\r\nSeven,Eight,Nine\r\n";
$it = Csv\LazyLaxCollection::createFromBuffer($string, ',', 'あ');

try {
    foreach ($it as $nb => $row) {
        var_dump($nb, $row);
    }
} catch (\Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
int(0)
array(3) {
  [0]=>
  string(3) "One"
  [1]=>
  string(3) "Two"
  [2]=>
  string(5) "Three"
}
ValueError: Enclosure sequence is used in a non escaped field
int(0)
array(3) {
  [0]=>
  string(3) "One"
  [1]=>
  string(3) "Two"
  [2]=>
  string(5) "Three"
}
ValueError: Enclosure sequence is used in a non escaped field
