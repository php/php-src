--TEST--
GitLab bug #16 (Unbounded enclosure lookahead remains after the 2022 buffer-overflow fix)
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

var_dump(Csv\row_to_array(str_repeat('a', 7), ',', str_repeat('a', 200), "\n"));

?>
--EXPECT--
array(1) {
  [0]=>
  string(7) "aaaaaaa"
}
