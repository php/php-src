--TEST--
comparing objects with strings/NULL
--FILE--
<?php

$o=new StdClass;

var_dump($o == "");
var_dump($o != "");
var_dump($o <  "");
var_dump("" <  $o);
var_dump("" >  $o);
var_dump($o != null);
var_dump(is_null($o));

?>
===DONE===
--EXPECT--
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
===DONE===
