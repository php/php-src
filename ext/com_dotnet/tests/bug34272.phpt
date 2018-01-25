--TEST--
Bug #34272 (empty array onto COM object blows up)
--SKIPIF--
<?php
if (!extension_loaded("com_dotnet")) print "skip COM/.Net support not present"; ?>
--FILE--
<?php
error_reporting(E_ALL);

try {
	$dict = new COM("Scripting.Dictionary");
	$dict->add('foo', array());
	print sizeof($dict['foo'])."\n";
	$dict->add('bar', array(23));
	print sizeof($dict['bar'])." \n";
} catch (Exception $e) {
	print $e;
}
?>
--EXPECT--
0
1
