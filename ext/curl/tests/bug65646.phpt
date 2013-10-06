--TEST--
Bug #65646 (re-enable CURLOPT_FOLLOWLOCATION with open_basedir or safe_mode): open_basedir disabled
--SKIPIF--
<?php
if (!extension_loaded('curl')) exit("skip curl extension not loaded");
if (ini_get('open_basedir')) exit("skip open_basedir is set");
?>
--FILE--
<?php
$ch = curl_init();
var_dump(curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true));
curl_close($ch);
?>
--EXPECT--
bool(true)
