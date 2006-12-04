--TEST--
unixtojd()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo unixtojd(40000). "\n";
echo unixtojd(1000000000). "\n";
echo unixtojd(1152459009). "\n";
?>
--EXPECT--
2440588
2452161
2453926
