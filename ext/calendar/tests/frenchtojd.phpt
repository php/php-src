--TEST--
frenchtojd()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo frenchtojd(-1,-1,-1), "\n";
echo frenchtojd(0,0,0), "\n";
echo frenchtojd(1,1,1), "\n";
echo frenchtojd(14,31,15), "\n";
?>
--EXPECT--
0
0
2375840
0
