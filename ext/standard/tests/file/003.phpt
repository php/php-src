--TEST--
is_file/file_exist
--POST--
--GET--
--FILE--
<?php require '003.inc' ?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
