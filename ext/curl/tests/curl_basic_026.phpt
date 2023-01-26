--TEST--
Test curl_getinfo() function with CURLINFO_* and CURLOPT_* from curl >= 7.81.0
--INI--
open_basedir=.
--EXTENSIONS--
curl
--SKIPIF--
<?php $curl_version = curl_version();
if ($curl_version['version_number'] < 0x075100) {
        exit("skip: test works only with curl >= 7.81.0");
}
?>
--FILE--
<?php

include 'server.inc';

$ch = curl_init();
var_dump(array_key_exists('capath', curl_getinfo($ch)));
var_dump(array_key_exists('cainfo', curl_getinfo($ch)));

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
var_dump(curl_setopt($ch, CURLOPT_MIME_OPTIONS, CURLMIMEOPT_FORMESCAPE));
// NOTE: depends on whether websocket support is compiled in
//var_dump(curl_setopt($ch, CURLOPT_WS_OPTIONS, CURLWS_RAW_MODE));
var_dump(curl_setopt($ch, CURLOPT_PROTOCOLS_STR, "FilE,DICT"));
var_dump(curl_setopt($ch, CURLOPT_PROTOCOLS_STR, "DICT"));
var_dump(curl_setopt($ch, CURLOPT_REDIR_PROTOCOLS_STR, "HTTP"));
var_dump(curl_setopt($ch, CURLOPT_CA_CACHE_TIMEOUT, 1));
var_dump(curl_setopt($ch, CURLOPT_QUICK_EXIT, 1000));
var_dump(curl_setopt($ch, CURLOPT_SSH_HOSTKEYFUNCTION, function ($keytype, $key, $keylen) {
    // Can't really trigger this in a test
    var_dump($keytype);
    var_dump($key);
    var_dump($keylen);
}));
curl_exec($ch);
curl_close($ch);
?>
--EXPECTF--
bool(true)
bool(true)
bool(true)

Warning: curl_setopt(): The FILE protocol cannot be activated when an open_basedir is set in %s on line %d
bool(false)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
