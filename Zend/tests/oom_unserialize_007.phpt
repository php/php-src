--TEST--
OOM unserialize() classname
--INI--
memory_limit=200M
error_reporting=E_ALL&~E_DEPRECATED
--FILE--
<?php

$classname = str_repeat('a', 32*1024*1024);
$payload = 'O:'.strlen($classname).':"'.$classname.'":0:{}';

ini_set('memory_limit', memory_get_usage() + 20*1024*1024);
var_dump(gettype(unserialize($payload)));

?>
--EXPECTF--
Fatal error: Allowed memory size of %d bytes exhausted %S(tried to allocate %d bytes) in %s on line 7
