--TEST--
strtr() function
--FILE--
<?php
$trans = array("hello"=>"hi", "hi"=>"hello", "a"=>"A", "world"=>"planet");
var_dump(strtr("# hi all, I said hello world! #", $trans));
?>
--EXPECT--
string(32) "# hello All, I sAid hi planet! #"
