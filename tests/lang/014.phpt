--TEST--
Testing eval function inside user-defined function
--POST--
--GET--
--FILE--
<?php 
function F ($a) { 
	eval($a);
}

error_reporting(0);
F("echo \"Hello\";");
?>
--EXPECT--
Hello
