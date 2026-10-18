--TEST--
OOM unserialize() classname (2)
--INI--
memory_limit=200M
error_reporting=E_ALL&~E_DEPRECATED
--FILE--
<?php

// Use a classname with uppercase chars, so a lowercase copy is needed. We OOM
// while creating the copy.
$classname = str_repeat('A', 32*1024*1024);
$payload = 'O:'.strlen($classname).':"'.$classname.'":0:{}';

ini_set('memory_limit', memory_get_usage() + 40*1024*1024);
var_dump(gettype(unserialize($payload)));

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 9
