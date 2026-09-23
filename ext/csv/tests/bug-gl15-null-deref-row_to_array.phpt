--TEST--
GitLab bug #15 (NULL dereference when rowToArray() emits an empty field)
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

var_dump(Csv\row_to_array(','));

?>
--EXPECT--
array(2) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
}
