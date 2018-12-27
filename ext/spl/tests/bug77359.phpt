--TEST--
Bug #77359: spl_autoload causes segfault
--FILE--
<?php

$str = "foo";
$str .= "bar";
spl_autoload($str);
spl_autoload($str);
var_dump($str);

?>
--EXPECT--
string(6) "foobar"
