--TEST--
Bug #73350 (Exception::__toString() cause circular references)
--FILE--
<?php
$e = new Exception();

// This line cause problem :(
// Comment it to see the difference.
(string) $e;

// This line show the clue (PHP Warning: ...).
var_export($e);
?>
--EXPECTF--
Exception::__set_state(array(
   'message' => '',
   'string' => 'Exception in %sbug73350.php:%d
Stack trace:
#0 {main}',
   'code' => 0,
   'file' => '%sbug73350.php',
   'line' => %d,
   'trace' => 
  array (
  ),
   'previous' => NULL,
))
