--TEST--
Yield without value in reference generator function should create notice
--FILE--
<?php

function &y()
{
  yield null;
  yield;
}
foreach (y() as &$y);

?>
--EXPECTF--
Notice: Only variable references should be yielded by reference in %s on line %d

Notice: Only variable references should be yielded by reference in %s on line %d
