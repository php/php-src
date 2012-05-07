--TEST--
Bug #61948 (CURLOPT_COOKIEFILE '' raises open_basedir restriction)
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; ?>
--INI--
open_basedir="/tmp"
--FILE--
<?php
  $ch = curl_init();
  var_dump(curl_setopt($ch, CURLOPT_COOKIEFILE, ""));
  var_dump(curl_setopt($ch, CURLOPT_COOKIEFILE, "/tmp/foo"));
  var_dump(curl_setopt($ch, CURLOPT_COOKIEFILE, "/xxx/bar"));
  curl_close($ch);
?>
--EXPECTF--
bool(false)
bool(true)

Warning: curl_setopt(): open_basedir restriction in effect. File(/xxx/bar) is not within the allowed path(s): (/tmp) in %sbug61948.php on line %d
bool(false)
