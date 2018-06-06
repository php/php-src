--TEST--
Validation of class names in the autoload process
--FILE--
<?php
spl_autoload_register(function ($name) {
	echo "$name\n";
});
$a = "../BUG";
$x = new $a;
echo "BUG\n";
?>
--EXPECTF--
Fatal error: Uncaught Error: Class '../BUG' not found in %sautoload_021.php:6
Stack trace:
#0 {main}
  thrown in %sautoload_021.php on line 6
