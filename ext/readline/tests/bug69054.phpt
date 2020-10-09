--TEST--
Bug #69054 (Null dereference in readline_(read|write)_history() without parameters)
--SKIPIF--
<?php if (!extension_loaded("readline") || !function_exists('readline_add_history')) die("skip"); ?>
--INI--
open_basedir="{TMP}"
--FILE--
<?php readline_read_history(); ?>
==DONE==
--EXPECT--
==DONE==
