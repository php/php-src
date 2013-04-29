--TEST--
Bug #61948 (CURLOPT_COOKIEFILE '' raises open_basedir restriction)
--SKIPIF--
<?php if (!extension_loaded("curl")) print "skip"; 
if(substr(PHP_OS, 0, 3) != 'WIN' )
  die("skip Not Valid for Linux");
?>
--INI--
open_basedir="c:/tmp"
--FILE--
<?php
  $ch = curl_init();
  var_dump(curl_setopt($ch, CURLOPT_COOKIEFILE, ""));
  var_dump(curl_setopt($ch, CURLOPT_COOKIEFILE, "c:/tmp/foo"));
  var_dump(curl_setopt($ch, CURLOPT_COOKIEFILE, "c:/xxx/bar"));
  curl_close($ch);
?>
--EXPECTF--
%a

Warning: curl_setopt(): open_basedir restriction in effect. File(c:/tmp/foo) is not within the allowed path(s): (c:/tmp) in %sbug61948-win32.php on line %d
bool(false)

Warning: curl_setopt(): open_basedir restriction in effect. File(c:/xxx/bar) is not within the allowed path(s): (c:/tmp) in %sbug61948-win32.php on line %d
bool(false)
