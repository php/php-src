--TEST--
is_*() and file_exists() return values are boolean.
--POST--
--GET--
--FILE--
<?php require '003.inc' ?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
