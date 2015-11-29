--TEST--
output buffering - fatalism
--FILE--
<?php
function obh($s)
{
	return ob_get_flush();
}
ob_start("obh");
echo "foo\n";
?>
--EXPECTF--
Fatal error: ob_get_flush(): Cannot use output buffering in output buffering display handlers in %sob_011.php on line %d
