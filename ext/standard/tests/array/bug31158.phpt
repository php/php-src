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
Notice: Undefined variable: GLOBALS in %sbug31158.php on line 6

Warning: Variable of type null does not accept array offsets in %s on line %d
ok

