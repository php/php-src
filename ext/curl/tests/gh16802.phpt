--TEST--
GH-16802 (open_basedir bypass using curl extension)
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (PHP_OS_FAMILY === "Windows") die("skip not for Windows");
?>
--INI--
open_basedir=/nowhere
--FILE--
<?php
$ch = curl_init("file:///etc/passwd");
curl_setopt($ch, CURLOPT_PROTOCOLS_STR, "all");
curl_setopt($ch, CURLOPT_PROTOCOLS_STR, "ftp,all");
curl_setopt($ch, CURLOPT_PROTOCOLS_STR, "all,ftp");
curl_setopt($ch, CURLOPT_PROTOCOLS_STR, "all,file,ftp");
var_dump(curl_exec($ch));
?>
--EXPECTF--
Warning: curl_setopt(): The FILE protocol cannot be activated when an open_basedir is set in %s on line %d

Warning: curl_setopt(): The FILE protocol cannot be activated when an open_basedir is set in %s on line %d

Warning: curl_setopt(): The FILE protocol cannot be activated when an open_basedir is set in %s on line %d

Warning: curl_setopt(): The FILE protocol cannot be activated when an open_basedir is set in %s on line %d
bool(false)
