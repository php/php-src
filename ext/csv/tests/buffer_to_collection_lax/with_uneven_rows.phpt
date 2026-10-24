--TEST--
Error conditions for Csv\buffer_to_collection_lax()
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
    ],
    [
        'Seven',
        'Eight',
        'Nine',
    ],
];

$string = "One,Two,Three\r\nFour,Five\r\nSeven,Eight,Nine\r\n";

var_dump($collection === Csv\buffer_to_collection_lax($string));
var_dump(Csv\buffer_to_collection_lax($string));

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
  array(2) {
    [0]=>
    string(4) "Four"
    [1]=>
    string(4) "Five"
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
