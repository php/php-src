--TEST--
jdtofrench()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo jdtofrench(0). "\n";
echo jdtofrench(2375840). "\n";
echo jdtofrench(2375850). "\n";
echo jdtofrench(2375940). "\n";
echo jdtofrench(2376345). "\n";
echo jdtofrench(2385940). "\n";
?>
--EXPECT--
0/0/0
1/1/1
1/11/1
4/11/1
5/21/2
0/0/0
