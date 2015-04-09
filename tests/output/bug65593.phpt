--TEST--
Bug #65593 (ob_start(function(){ob_start();});)
--FILE--
<?php 
echo "Test\n";

ob_start(function(){ob_start();});
?>
===DONE===
--EXPECTF--
Test

Catchable fatal error: ob_start(): Cannot use output buffering in output buffering display handlers in %sbug65593.php on line %d
