--TEST--
Bug #26198 (strtotime handling of "M Y" and "Y M" format)
--FILE--
<?php
	echo date("F Y\n", strtotime("Oct 2001"));
	echo date("M Y\n", strtotime("2001 Oct"));
?>
--EXPECT--
October 2001
Oct 2001
