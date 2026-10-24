--TEST--
curl_setopt(CURLOPT_SSLENGINE) should respect open_basedir for engine paths
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (!defined('CURLOPT_SSLENGINE')) {
    die('skip CURLOPT_SSLENGINE not available');
}

$version = curl_version();

if (!isset($version['ssl_version']) ||
    stripos($version['ssl_version'], 'OpenSSL') === false) {
    die('skip requires libcurl built against OpenSSL');
}
?>
--INI--
open_basedir=.
display_errors=1
log_errors=0
--FILE--
<?php

$ch = curl_init();
var_dump(curl_setopt($ch, CURLOPT_SSLENGINE, '/dev/null'));
var_dump(curl_setopt($ch, CURLOPT_SSLENGINE, __FILE__));
var_dump(curl_setopt($ch, CURLOPT_SSLENGINE, 'someengine:property'));

?>
--EXPECTF--
Warning: curl_setopt(): open_basedir restriction in effect. File(/dev/null) is not within the allowed path(s): (.) in %s on line %d
bool(false)
bool(false)
bool(false)
