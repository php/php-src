--TEST--
Csv\row_to_array() buffer overflow bug
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$string = "key1,あああああああああああああああああああああああああああああああああkey2あああああああああああああああああああああああああああああああああ,key3";
var_dump(Csv\row_to_array($string, ',', 'あああああああああああああああああああああああああああああああああ'));

$string = "One,Two,Three\r\nFour,あああああああああああああああああああああああああああああああああFiveあああああああああああああああああああああああああああああああああ,Six\r\nSeven,Eight,Nine\r\n";
var_dump(Csv\buffer_to_collection_lax($string, ',', 'あああああああああああああああああああああああああああああああああ'));
var_dump(Csv\buffer_to_collection($string, ',', 'あああああああああああああああああああああああああああああああああ'));

$string = "One,Two,Three\r\nFour,あああああああああああああああああああああああああああああああああFiveあああああああああああああああああああああああああああああああああ,Six\r\nSeven,Eight,Nine";
var_dump(Csv\buffer_to_collection_lax($string, ',', 'あああああああああああああああああああああああああああああああああ'));
var_dump(Csv\buffer_to_collection($string, ',', 'あああああああああああああああああああああああああああああああああ'));

?>
--EXPECT--
array(3) {
  [0]=>
  string(4) "key1"
  [1]=>
  string(4) "key2"
  [2]=>
  string(4) "key3"
}
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
