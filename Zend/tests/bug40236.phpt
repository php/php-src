--TEST--
Bug #40236 (php -a function allocation eats memory)
--SKIPIF--
if (php_sapi_name() != "cli") die("skip CLI only");
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
$cmd = "$php -d memory_limit=4M -a ".dirname(__FILE__)."/bug40236.inc";
echo `$cmd`;
?>
--EXPECTF--
Interactive %s

ok
