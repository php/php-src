--TEST--
easter_days()
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
echo easter_days(1999), "\n";
echo easter_days(1492), "\n";
echo easter_days(1913), "\n";
?>
--EXPECT--
14
32
2
