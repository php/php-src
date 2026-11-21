--TEST--
memory_hard_limit_percent runtime override and restore
--INI--
memory_hard_limit_percent=150
--FILE--
<?php
var_dump(ini_get('memory_hard_limit_percent'));
var_dump(ini_set('memory_hard_limit_percent', '125'));
var_dump(ini_get('memory_hard_limit_percent'));
ini_restore('memory_hard_limit_percent');
var_dump(ini_get('memory_hard_limit_percent'));
?>
--EXPECT--
string(3) "150"
string(3) "150"
string(3) "125"
string(3) "150"
