--TEST--
strtr() function
--FILE--
<?php
$trans = array("hello" => "hi", "hi" => "hello","a" => "A","world" => "planet");
echo strtr("# hi all, I said hello world! #", $trans) . "\n";
?>
--EXPECT--
# hello All, I sAid hi planet! #
