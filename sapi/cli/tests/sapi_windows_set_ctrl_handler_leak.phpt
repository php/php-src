--TEST--
sapi_windows_set_ctrl_handler() leak bug
--SKIPIF--
<?php
include "skipif.inc";

if (strtoupper(substr(PHP_OS, 0, 3)) !== 'WIN')
  die("skip this test is for Windows platforms only");
?>
--FILE--
<?php

class Test {
	public function set() {
		sapi_windows_set_ctrl_handler(self::cb(...));
	}
	public function cb() {
	}
}

$test = new Test;
$test->set();

echo "Done\n";

?>
--EXPECT--
Done
