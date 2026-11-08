--TEST--
GH-25249 (A fiber starts at the default error_reporting when the directive is unset)
--FILE--
<?php
$php_escaped = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
echo shell_exec($php_escaped . ' -n ' . escapeshellarg(__DIR__ . '/gh25249.inc'));
?>
--EXPECTF--
30719

Warning: Undefined variable $main in %s on line 3
30719

Warning: Undefined variable $fiber in %s on line 6
