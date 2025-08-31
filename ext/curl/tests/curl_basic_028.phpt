--TEST--
Test curl_getinfo() function with CURLOPT_* from curl >= 7.85.0
--INI--
open_basedir=.
--EXTENSIONS--
curl
--SKIPIF--
<?php $curl_version = curl_version();
if ($curl_version['version_number'] < 0x075500) {
    exit("skip: test works only with curl >= 7.85.0");
}
?>
--FILE--
<?php
include 'server.inc';

$ch = curl_init();

$host = curl_cli_server_start();

$url = "{$host}/get.inc?test=";
curl_setopt($ch, CURLOPT_URL, $url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
var_dump(curl_setopt($ch, CURLOPT_PROTOCOLS_STR, "FilE,DICT"));
var_dump(curl_setopt($ch, CURLOPT_PROTOCOLS_STR, "DICT"));
var_dump(curl_setopt($ch, CURLOPT_REDIR_PROTOCOLS_STR, "HTTP"));
curl_exec($ch);
?>
--EXPECTF--
Warning: curl_setopt(): The FILE protocol cannot be activated when an open_basedir is set in %s on line %d
bool(false)
bool(true)
bool(true)
