--TEST--
Error messages are shown
--FILE--
<?php
// If this test fails ask the developers of run-test.php
ini_set("zend.assertions", -1);
?>
--EXPECTREGEX--
.*enabled or disabled.*
