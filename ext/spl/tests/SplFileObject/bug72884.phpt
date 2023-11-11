--TEST--
Bug #72884:  isCloneable() on SplFileObject should return false
--FILE--
<?php
$x=new SplFileObject(__FILE__);
$r=new ReflectionObject($x);
var_dump($r->isCloneable());

?>
--EXPECT--
bool(false)
