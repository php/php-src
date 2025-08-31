--TEST--
Crash on $x['2x']['y'] += 1 when $x is string
--FILE--
<?php
$x = "a";
$x['2x']['y'] += 1;

echo "Done\n";
?>
--EXPECTF--
Warning: Illegal string offset "2x" in %s on line %d

Fatal error: Uncaught Error: Cannot use string offset as an array in %soffset_assign.php:%d
Stack trace:
#0 {main}
  thrown in %soffset_assign.php on line %d
