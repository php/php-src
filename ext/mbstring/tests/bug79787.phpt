--TEST--
Bug #79787 mb_strimwidth does not trim string
--EXTENSIONS--
mbstring
--FILE--
<?php
echo mb_strimwidth("一二三", 0, 4, '.', 'UTF-8')."\n";
echo mb_strimwidth("一二三", 0, 5, '.', 'UTF-8')."\n";
echo mb_strimwidth("一二三", 0, 6, '.', 'UTF-8')."\n";
echo mb_strimwidth("abcdef", 0, 4, '.', 'UTF-8')."\n";
echo mb_strimwidth("abcdef", 0, 5, '.', 'UTF-8')."\n";
echo mb_strimwidth("abcdef", 0, 6, '.', 'UTF-8')."\n";
?>
--EXPECT--
一.
一二.
一二三
abc.
abcd.
abcdef
