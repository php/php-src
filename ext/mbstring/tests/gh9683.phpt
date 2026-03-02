--TEST--
GH-9683 (Problem when ISO-2022-JP-MS is specified in mb_ encode_mimeheader)
--EXTENSIONS--
mbstring
--FILE--
<?php

echo mb_decode_mimeheader(mb_encode_mimeheader('１２３４５６７８９０１２３４５６７８９０１２', 'ISO-2022-JP-MS', 'B'));
?>
--EXPECT--
１２３４５６７８９０１２３４５６７８９０１２
