--TEST--
Bug #70330 (Segmentation Fault with multiple "curl_copy_handle")
--SKIPIF--
<?php include 'skipif.inc'; ?>
--FILE--
<?php
$t2 = curl_init();
$t3 = curl_copy_handle($t2);
$t3 = curl_copy_handle($t2);
$t4 = curl_init();
$t3 = curl_copy_handle($t4);
$t5 = curl_init();
$t6 = curl_copy_handle($t5);
?>
okey
--EXPECT--
okey
