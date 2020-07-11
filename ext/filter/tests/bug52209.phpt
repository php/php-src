--TEST--
Bug #52209 (INPUT_ENV returns NULL for set variables (CLI))
--SKIPIF--
<?php
/* This test makes no sense on windows as an empty variable
   would never show up in the "set" list. Which means, it's
   always undefined in PHP. */
if(substr(PHP_OS, 0, 3) == "WIN") die("skip Not for Windows");
if (!extension_loaded("filter")) die ('skip filter extension not loaded');
if (empty($_ENV['PWD'])) die('skip PWD is empty');
?>
--INI--
variables_order=GPCSE
--FILE--
<?php
    var_dump(filter_input(INPUT_ENV, 'PWD'));
?>
--EXPECTF--
string(%d) "%s"
