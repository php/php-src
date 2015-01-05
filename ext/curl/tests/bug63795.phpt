--TEST--
Bug #63795 (CURL >= 7.28.0 no longer support value 1 for CURLOPT_SSL_VERIFYHOST)
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
        exit("skip curl extension not loaded");
}
$curl_version = curl_version();
if ($curl_version['version_number'] < 0x071c01) {
        exit("skip: test valid for libcurl >= 7.28.1");
}
?>
--FILE--
<?php
$ch = curl_init();
var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, false));
/* Case that should throw an error */
var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, true));
var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 0));
var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 1));
var_dump(curl_setopt($ch, CURLOPT_SSL_VERIFYHOST, 2));

curl_close($ch);
?>
--EXPECTF--
bool(true)

Notice: curl_setopt(): CURLOPT_SSL_VERIFYHOST no longer accepts the value 1, value 2 will be used instead in %s on line %d
bool(true)
bool(true)

Notice: curl_setopt(): CURLOPT_SSL_VERIFYHOST no longer accepts the value 1, value 2 will be used instead in %s on line %d
bool(true)
bool(true)
