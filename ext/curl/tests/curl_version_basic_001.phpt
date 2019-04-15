--TEST--
Test curl_version() basic functionality
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
    var_dump($info_curl["ssl_version"]);
    var_dump($info_curl["libz_version"]);
    var_dump(array_key_exists("protocols", $info_curl));
?>
--EXPECTF--
int(%i)
int(%i)
int(%i)
int(%i)
string(%i) "%s"
string(%i) "%s"
string(%i) "%s"
string(%i) "%s"
bool(true)
