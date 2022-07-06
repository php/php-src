--TEST--
Bug #73615 (phpdbg without option never load .phpdbginit at startup)
--SKIPIF--
<?php
if (!getenv('TEST_PHPDBG_EXECUTABLE')) die("SKIP: No TEST_PHPDBG_EXECUTABLE specified");
?>
--FILE--
<?php

$phpdbg = getenv('TEST_PHPDBG_EXECUTABLE');

chdir(__DIR__."/bug73615");

print `$phpdbg -qn`;

?>
--EXPECT--
Executed .phpdbginit
