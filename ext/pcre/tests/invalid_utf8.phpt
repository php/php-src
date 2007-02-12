--TEST--
preg_replace() and invalid UTF8
--SKIPIF--
<?php
if (@preg_match_all('/./u', "", $matches) === false) {
	die("skip no utf8 support in PCRE library");
}
?>
--FILE--
<?php

$string = urldecode("search%e4"); 
$result = preg_replace("#(&\#x*)([0-9A-F]+);*#iu","$1$2;",$string); 
var_dump($result); 
var_dump(preg_last_error());

echo "Done\n";
?>
--EXPECT--	
NULL
int(4)
Done
