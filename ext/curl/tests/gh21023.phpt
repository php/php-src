--TEST--
GH-21023 (crash with CURLOPT_XFERINFOFUNCTION set with an invalid callback)
--EXTENSIONS--
curl
--INI--
error_reporting = E_ALL & ~E_DEPRECATED
--FILE--
<?php
include 'server.inc';
$host = curl_cli_server_start();
$url = "{$host}/get.inc";
$ch = curl_init($url);
curl_setopt($ch, CURLOPT_NOPROGRESS, 0);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_XFERINFOFUNCTION, $callback);
curl_exec($ch);
$ch = curl_init($url);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_PROGRESSFUNCTION, $callback);
curl_exec($ch);
?>
--EXPECTF--
Warning: Undefined variable $callback in %s on line %d

Warning: Undefined variable $callback in %s on line %d
