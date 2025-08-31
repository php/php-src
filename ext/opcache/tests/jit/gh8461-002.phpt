--TEST--
Bug GH-8461 002 (JIT does not account for class re-compile)
--EXTENSIONS--
opcache
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit=1255
opcache.file_update_protection=0
opcache.revalidate_freq=0
opcache.protect_memory=1
--FILE--
<?php

// Checks that JITed code does not crash in --repeat 2 after the UniqueList
// class is recompiled.

require __DIR__ . '/gh8461-002.inc';

for ($i = 0; $i < 10; $i++) {
    UniqueList::foo();
}

// mark the file as changed (important)
touch(__DIR__ . '/gh8461-002.inc');

print "OK";
?>
--EXPECT--
OK
