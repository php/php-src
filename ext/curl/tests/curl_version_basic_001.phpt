--TEST--
Test curl_version() basic functionality
--CREDITS--
Rodrigo Prado de Jesus <royopa [at] gmail [dot] com>
#PHPSP User Group Brazil
--SKIPIF--
<?php
    if (!extension_loaded("curl")) {
        exit("skip curl extension not loaded");
    }
?>
--FILE--
<?php
    $info_curl = curl_version();
    var_dump($info_curl["version_number"]);
    var_dump($info_curl["age"]);
    var_dump($info_curl["features"]);
    var_dump($info_curl["ssl_version_number"]);
    var_dump($info_curl["version"]);
    var_dump($info_curl["host"]);
?>
--EXPECTF--
int(%i)
int(%i)
int(%i)
int(%i)
string(%i) "%s"
string(%i) "%s"
