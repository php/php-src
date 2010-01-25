--TEST--
Bug #44827 (define() allows :: in constant names)
--FILE--
<?php
define('foo::bar', 1);
define('::', 1);
?>
--EXPECTF--
Warning: Class constants cannot be defined or redefined in %sbug44827.php on line %d

Warning: Class constants cannot be defined or redefined in %sbug44827.php on line %d
