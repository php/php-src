--TEST--
output buffering - fatalism
--FILE--
<?php
function obh($s)
{
	print_r($s, 1);
}
ob_start("obh");
echo "foo\n";
?>
--EXPECTF--
Fatal error: print_r(): Cannot use output buffering in output buffering display handlers in %sob_010.php on line %d
