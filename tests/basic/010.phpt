--TEST--
Testing | and & operators
--POST--
--GET--
--FILE--
<?php $a=8; $b=4; $c=8; echo $a|$b&$c?>
--EXPECT--
8
