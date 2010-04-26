--TEST--
CURLOPT_FOLLOWLOCATION case check open_basedir
--CREDITS--
WHITE new media architects - Dennis
--INI--
open_basedir = DIRECTORY_SEPARATOR."tmp";
--SKIPIF--
<?php
if (!extension_loaded("curl")) print "skip cURL not loaded";
?>
--FILE--
<?php
print (ini_get("OPEN_BASEDIR"));
$ch = curl_init();
$succes = curl_setopt($ch, CURLOPT_FOLLOWLOCATION, 1);
curl_close($ch);
var_dump($succes);
?>
--EXPECTF--
Warning: curl_setopt(): CURLOPT_FOLLOWLOCATION cannot be activated when an open_basedir is set in %s.php on line %d
bool(false)

