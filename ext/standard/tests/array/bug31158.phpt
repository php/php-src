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
Fatal error: Uncaught Error: array_splice(): Argument #1 ($array) cannot be passed by reference in %s:%d
Stack trace:
#0 %s(%d): __()
#1 {main}
  thrown in %s on line %d
