--TEST--
oss-fuzz #60741: Leak in open_basedir
--INI--
open_basedir="{TMP}"
--FILE--
<?php
ini_set('open_basedir', ini_get('open_basedir'));
?>
--EXPECT--
