--TEST--
Testing eval function
--POST--
--GET--
--FILE--
<?php 
error_reporting(0);
$a="echo \"Hello\";";
eval($a);
?>
--EXPECT--
Hello
