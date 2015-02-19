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

Fatal error: Cannot destroy active lambda function in %sbug65593.php on line %d
