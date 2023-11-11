--TEST--
SplFileObject::fwrite function - writing with two parameters, length > input string length
--FILE--
<?php
$file = __DIR__.'/SplFileObject_fwrite_variation_002.txt';
if(file_exists($file)) {
    unlink($file);
}
$obj = New SplFileObject($file,'w');
$obj->fwrite('test_write',12);
var_dump(file_get_contents($file));
?>
--CLEAN--
<?php
$file = __DIR__.'/SplFileObject_fwrite_variation_002.txt';
if(file_exists($file)) {
	unlink($file);
}
?>
--EXPECT--
string(10) "test_write"
