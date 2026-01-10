--TEST--
Bug #62294: register_shutdown_function() does not handle exit code correctly
--FILE--
<?php

$php = getenv('TEST_PHP_EXECUTABLE_ESCAPED');
exec($php . ' -n ' . escapeshellarg(__DIR__ . '/bug62294.inc'), $output, $exit_status);
var_dump($exit_status);

?>
--EXPECT--
int(255)
