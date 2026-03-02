--TEST--
GH-12366: Use-after-free of constant name when script doesn't fit in SHM
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.file_update_protection=1
--FILE--
<?php
$file = __DIR__ . '/gh12366.inc';
// Update timestamp and use opcache.file_update_protection=1 to prevent included file from being persisted in shm.
touch($file);
require $file;
?>
--EXPECT--
enum(Level::Debug)
