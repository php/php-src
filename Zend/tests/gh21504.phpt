--TEST--
GH-21504: Incorrect RC-handling for ZEND_EXT_STMT op1
--FILE--
<?php

$php_escaped = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
$cmd = $php_escaped . ' -n -e ' . escapeshellarg(__DIR__ . '/gh21504.inc');
echo shell_exec($cmd);

?>
--EXPECT--
string(4) "1234"
