--TEST--
Bug #65275: Calling exit() in a shutdown function does not change the exit value in CLI
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE');
exec($php . ' ' . __DIR__ . '/bug65275.inc', $output, $exit_status);
var_dump($exit_status);

?>
--EXPECT--
int(111)
