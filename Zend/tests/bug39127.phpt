--TEST--
Bug #39127 (Old-style constructor fallbacks produce strange results)
--FILE--
<?php

class a { function a() { var_dump("a::a() called"); } } 
class b extends a {} 

$b = new b; 
var_dump(is_callable(array($b,"a")));
var_dump(is_callable(array($b,"b")));
var_dump(is_callable(array($b,"__construct")));

echo "Done\n";
?>
--EXPECTF--	
string(13) "a::a() called"
bool(true)
bool(false)
bool(false)
Done
