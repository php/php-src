--TEST--
Execute closures returned by the primary script (Included file returns closure, but primary script does not)
--SKIPIF--
<?php
include "skipif.inc";
?>
--FILE--
<?php

$closure = include "primary_script_closure.inc";

var_dump($closure);

?>
--EXPECTF--
object(Closure)#%d (3) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%sprimary_script_closure.inc"
  ["line"]=>
  int(3)
}
