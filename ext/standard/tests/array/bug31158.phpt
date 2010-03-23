--TEST--
Bug #31158 (array_splice on $GLOBALS crashes)
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
ok

