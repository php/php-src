--TEST--
SplFileObject::fwrite function - writing with two parameters length < input string length
--FILE--
<?php
$file = dirname(__FILE__).'/SplFileObject_fwrite_variation_001.txt';
if(file_exists($file)) {
	unlink($file);
}
$obj = New SplFileObject($file,'w');
$obj->fwrite('test_write',4);
var_dump(file_get_contents($file));
?>
--CLEAN--
<?php
$file = dirname(__FILE__).'/SplFileObject_fwrite_variation_001.txt';
if(file_exists($file)) {
	unlink($file);
}
?>
--EXPECT--
string(4) "test"
