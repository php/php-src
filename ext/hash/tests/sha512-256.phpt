--TEST--
sha512/256 algorithm
--SKIPIF--
<?php if(!extension_loaded("hash")) print "skip"; ?>
--FILE--
<?php
echo hash('sha512/256', '') . "\n";
echo hash('sha512/256', 'abc') . "\n";
echo hash('sha512/256', 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu') . "\n";
--EXPECT--
c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a
53048e2681941ef99b2e29b76b4c7dabe4c2d0c634fc6d46e0e2f13107e7af23
3928e184fb8690f840da3988121d31be65cb9d3ef83ee6146feac861e19b563a
