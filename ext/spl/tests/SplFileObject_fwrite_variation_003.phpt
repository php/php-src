--TEST--
SplFileObject::fwrite function - writing with magic_quotes_runtime ini set 
--FILE--
<?php
ini_set('magic_quotes_runtime',true);
$file = dirname(__FILE__).'/SplFileObject_fwrite_variation_002.txt';
if(file_exists($file)) {
	unlink($file);
}
$obj = New SplFileObject($file,'w');
$obj->fwrite('"test" \'write\'');
var_dump(file_get_contents($file));
if(file_exists($file)) {
	unlink($file);
}
?>
--EXPECT--
string(18) "\"test\" \'write\'"
