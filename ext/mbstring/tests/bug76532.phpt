--TEST--
Bug #76532 (Integer overflow and excessive memory usage in mb_strimwidth)
--SKIPIF--
<?php require 'skipif.inc'; ?>
--FILE--
<?php
$string_to_trim = '得很幸福。有一天，一个长得很丑的老人带着一只木马来到王';
$width = 2147483647;
var_dump(mb_strimwidth($string_to_trim, 0, $width));
?>
--EXPECT--
string(81) "得很幸福。有一天，一个长得很丑的老人带着一只木马来到王"
