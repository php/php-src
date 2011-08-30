--TEST--
Check for Yaf_Registry
--SKIPIF--
<?php if (!extension_loaded("yaf")) print "skip"; ?>
--FILE--
<?php 
$str = "Ageli Platform";

Yaf_Registry::set("name",  $str);
unset($str);

var_dump(Yaf_Registry::get("name"));
var_dump(Yaf_Registry::has("name"));

$name = "name";

Yaf_Registry::del($name);

var_dump(Yaf_Registry::get($name));

var_dump(Yaf_Registry::has($name));

?>
--EXPECT--
string(14) "Ageli Platform"
bool(true)
NULL
bool(false)
