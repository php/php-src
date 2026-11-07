--TEST--
Test Csv\buffer_to_collection() with custom EOL sequence as a nul byte
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

var_dump($collection === Csv\buffer_to_collection($string, ',', '"', "\0"));
var_dump(Csv\buffer_to_collection($string, ',', '"', "\0"));

?>
--EXPECT--
bool(true)
array(3) {
  [0]=>
  array(3) {
    [0]=>
    string(3) "One"
    [1]=>
    string(3) "Two"
    [2]=>
    string(5) "Three"
  }
  [1]=>
  array(3) {
    [0]=>
    string(4) "Four"
    [1]=>
    string(4) "Five"
    [2]=>
    string(3) "Six"
  }
  [2]=>
  array(3) {
    [0]=>
    string(5) "Seven"
    [1]=>
    string(5) "Eight"
    [2]=>
    string(4) "Nine"
  }
}
