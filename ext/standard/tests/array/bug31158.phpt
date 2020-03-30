--TEST--
Bug #31158 (array_splice on $GLOBALS crashes)
--INI--
error_reporting = E_ALL
--FILE--
<?php
function __(){
  $GLOBALS['a'] = "bug\n";
  array_splice($GLOBALS,0,count($GLOBALS));
  /* All global variables including $GLOBALS are removed */
  echo $GLOBALS['a'];
}
__();
echo "ok\n";
?>
--EXPECTF--
Warning: Undefined variable $GLOBALS in %s on line %d

Warning: Trying to access array offset on value of type null in %s on line %d
ok
