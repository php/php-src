--TEST--
Bug #65646 (re-enable CURLOPT_FOLLOWLOCATION with open_basedir or safe_mode): open_basedir enabled; curl >= 7.19.4
--INI--
open_basedir=.
--SKIPIF--
<?php
if (!extension_loaded('curl')) exit("skip curl extension not loaded");
?>
--FILE--
<?php
$ch = curl_init();
var_dump(curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true));
var_dump(curl_setopt($ch, CURLOPT_PROTOCOLS, CURLPROTO_FILE));
var_dump(curl_setopt($ch, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_FILE));
curl_close($ch);
?>
--EXPECTF--
bool(true)

Warning: curl_setopt(): CURLPROTO_FILE cannot be activated when an open_basedir is set in %s on line %d
bool(false)

Warning: curl_setopt(): CURLPROTO_FILE cannot be activated when an open_basedir is set in %s on line %d
bool(false)
