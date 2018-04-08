--TEST--
sha512/224 algorithm
--SKIPIF--
<?php if(!extension_loaded("hash")) print "skip"; ?>
--FILE--
<?php
echo hash('sha512/224', '') . "\n";
echo hash('sha512/224', 'abc') . "\n";
echo hash('sha512/224', 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu') . "\n";
--EXPECT--
6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4
4634270f707b6a54daae7530460842e20e37ed265ceee9a43e8924aa
23fec5bb94d60b23308192640b0c453335d664734fe40e7268674af9
