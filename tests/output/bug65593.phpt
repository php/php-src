--TEST--
Bug #65593 (ob_start(function(){ob_start();});)
--FILE--
<?php 
echo "Test\n";

ob_start(function(){ob_start();});
?>
===DONE===
--EXPECT--
Test

Fatal error: Cannot destroy active lambda function in /home/mike/src/php-5.5/tests/output/bug65593.php on line 4
