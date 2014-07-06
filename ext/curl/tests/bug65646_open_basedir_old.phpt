--TEST--
Bug #65646 (re-enable CURLOPT_FOLLOWLOCATION with open_basedir or safe_mode): open_basedir enabled; curl < 7.19.4
--INI--
open_basedir=.
--SKIPIF--
<?php
if (!extension_loaded('curl')) exit("skip curl extension not loaded");
if (version_compare(curl_version()['version'], '7.19.4', '>=')) exit("skip curl version is too new");
?>
--FILE--
<?php
$ch = curl_init();
var_dump(curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true));
curl_close($ch);
?>
--EXPECTF--
Warning: curl_setopt(): CURLOPT_FOLLOWLOCATION cannot be activated when an open_basedir is set in %s on line %d
bool(false)
