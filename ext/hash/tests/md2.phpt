--TEST--
Hash: md2 algorithm
--FILE--
<?php
echo hash('md2', '') . "\n";
echo hash('md2', 'a') . "\n";
echo hash('md2', 'abc') . "\n";
echo hash('md2', 'message digest') . "\n";
echo hash('md2', 'abcdefghijklmnopqrstuvwxyz') . "\n";
echo hash('md2', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789') . "\n";
echo hash('md2', '12345678901234567890123456789012345678901234567890123456789012345678901234567890') . "\n";
--EXPECT--
8350e5a3e24c153df2275c9f80692773
32ec01ec4a6dac72c0ab96fb34c0b5d1
da853b0d3f88d99b30283a69e6ded6bb
ab4f496bfb2a530b219ff33031fe06b0
4e8ddff3650292ab5a4108c3aa47940b
da33def2a42df13975352846c30338cd
d5976f79d83d3a0dc9806c3c66f3efd8
