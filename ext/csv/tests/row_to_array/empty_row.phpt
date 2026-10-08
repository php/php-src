--TEST--
Test Csv\row_to_array() with an empty row is a single empty field
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

var_dump(Csv\row_to_array(''));
var_dump(Csv\row_to_array('', ';', "'", "\n"));
var_dump(Csv\row_to_array("\r\n"));
var_dump(Csv\array_to_row(Csv\row_to_array('')));

?>
--EXPECT--
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
array(1) {
  [0]=>
  string(0) ""
}
string(2) "
"
