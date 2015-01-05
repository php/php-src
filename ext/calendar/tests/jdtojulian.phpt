--TEST--
jdtojulian()
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
echo jdtojulian(0). "\n";
echo jdtojulian(2298874). "\n";
echo jdtojulian(2299151). "\n";
echo jdtojulian(2440588). "\n";
echo jdtojulian(2816423). "\n";
?>
--EXPECT--
0/0/0
12/22/1581
9/25/1582
12/19/1969
12/12/2998