--TEST--
GH-12962 (Double free of init_file in phpdbg_prompt.c)
--SKIPIF--
<?php
if (!getenv('TEST_PHPDBG_EXECUTABLE')) die("SKIP: No TEST_PHPDBG_EXECUTABLE specified");
?>
--FILE--
<?php
putenv('PHP_INI_SCAN_DIR='.__DIR__."/gh12962");
passthru($_ENV['TEST_PHPDBG_EXECUTABLE'] . " -q");
?>
--EXPECT--
Executed .phpdbginit
