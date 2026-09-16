--TEST--
Bug #60978 (exit code incorrect)
--FILE--
<?php
exec(getenv('TEST_PHP_EXECUTABLE_ESCAPED') . ' -n -r "exit(2);"', $output, $exit_code);
echo $exit_code;
?>
--EXPECT--
2
