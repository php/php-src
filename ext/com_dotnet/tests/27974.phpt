--TEST--
COM: mapping a safearray
--SKIPIF--
<?php # vim:ft=php
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present"; ?>
--FILE--
<?php
error_reporting(E_ALL);

try {
	$v = new VARIANT(array("123", "456", "789"));
	var_dump($v);
	print $v[0] . "\n";
	print $v[1] . "\n";
	print $v[2] . "\n";
	$v[1] = "hello";
	foreach ($v as $item) {
		var_dump($item);
	}
	try {
		$v[3] = "shouldn't work";
	} catch (com_exception $e) {
		if ($e->getCode() != DISP_E_BADINDEX) {
			throw $e;
		}
		echo "Got BADINDEX exception OK!\n";
	}
	echo "OK!";
} catch (Exception $e) {
	print $e;
}
?>
--EXPECT--
object(variant)#1 (0) {
}
123
456
789
string(3) "123"
string(5) "hello"
string(3) "789"
Got BADINDEX exception OK!
OK!
