--TEST--
Bug #69054 (Null dereference in readline_(read|write)_history() without parameters)
--EXTENSIONS--
readline
--INI--
open_basedir="{TMP}"
--FILE--
<?php readline_read_history(); ?>
==DONE==
--EXPECT--
==DONE==
