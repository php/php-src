--TEST--
Testing eval function inside user-defined function
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
