--TEST--
Test function posix_setegid() by calling it with its expected arguments
--SKIPIF--
<?php
    if(!extension_loaded("posix")) die("skip - POSIX extension not loaded");
?>
--FILE--
<?php
var_dump(posix_setegid(posix_getegid()));
?>
--EXPECT--
bool(true)
