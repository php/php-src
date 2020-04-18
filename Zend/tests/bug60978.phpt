--TEST--
Bug #60978 (exit code incorrect)
--FILE--
<?php
$php = getenv('TEST_PHP_EXECUTABLE');
exec($php . ' -n -r "exit(2);"', $output, $exit_code);
echo $exit_code;
?>
--EXPECT--
2
