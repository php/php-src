--TEST--
Bug #69054 (Null dereference in readline_(read|write)_history() without parameters)
--EXTENSIONS--
readline
--SKIPIF--
<?php if (!function_exists('readline_add_history')) die("skip"); ?>
--INI--
open_basedir="{TMP}"
--FILE--
<?php readline_read_history(); ?>
==DONE==
--EXPECT--
==DONE==
