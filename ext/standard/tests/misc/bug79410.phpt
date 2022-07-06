--TEST--
Bug #79410 (system() swallows last chunk if it is exactly 4095 bytes without newline)
--FILE--
<?php
ob_start();
system(getenv('TEST_PHP_EXECUTABLE') . ' -n -r "echo str_repeat(\".\", 4095);"');
var_dump(strlen(ob_get_clean()));
?>
--EXPECT--
int(4095)
