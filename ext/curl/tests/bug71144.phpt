--TEST--
Bug #71144 (Sementation fault when using cURL with ZTS)
--SKIPIF--
<?php include 'skipif.inc'; ?>
<?php if (!PHP_ZTS) { print "skip only for zts build"; } ?>
--FILE--
<?php
$ch = curl_init();
var_dump(curl_setopt($ch, CURLOPT_DNS_USE_GLOBAL_CACHE, 1));
?>
--EXPECTF--
Warning: curl_setopt(): CURLOPT_DNS_USE_GLOBAL_CACHE cannot be activated when thread safety is enabled in %sbug71144.php on line %d
bool(false)
