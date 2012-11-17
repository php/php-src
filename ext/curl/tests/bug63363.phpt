--TEST--
Bug #63363 (CURL silently accepts boolean value for SSL_VERIFYHOST)
--SKIPIF--
<?php
if (!extension_loaded("curl")) {
        exit("skip curl extension not loaded");
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

Notice: curl_setopt(): CURLOPT_SSL_VERIFYHOST set to true which disables common name validation (setting CURLOPT_SSL_VERIFYHOST to 2 enables common name validation) in %s on line %d
bool(true)
bool(true)
bool(true)
bool(true)
