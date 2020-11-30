--TEST--
Bug #73615 (phpdbg without option never load .phpdbginit at startup)
--SKIPIF--
<?php
if (!getenv('TEST_PHPDBG_EXECUTABLE')) die("SKIP: No TEST_PHPDBG_EXECUTABLE specified");
if (ini_get('opcache.jit') && ini_get('opcache.jit_buffer_size')) {
    die('skip phpdbg is incompatible with JIT');
}
?>
--FILE--
<?php

$phpdbg = getenv('TEST_PHPDBG_EXECUTABLE');

chdir(__DIR__."/bug73615");

print `$phpdbg -qn`;

?>
--EXPECT--
Executed .phpdbginit
