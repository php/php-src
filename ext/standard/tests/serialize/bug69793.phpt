--TEST--
Bug #69793: Remotely triggerable stack exhaustion via recursive method calls
--FILE--
<?php
$e = unserialize('O:9:"Exception":7:{s:17:"'."\0".'Exception'."\0".'string";s:1:"a";s:7:"'."\0".'*'."\0".'code";i:0;s:7:"'."\0".'*'."\0".'file";R:1;s:7:"'."\0".'*'."\0".'line";i:1337;s:16:"'."\0".'Exception'."\0".'trace";a:0:{}s:19:"'."\0".'Exception'."\0".'previous";i:10;s:10:"'."\0".'*'."\0".'message";N;}');

var_dump($e."");
?>
--EXPECTF--
Warning: Undefined property: Exception::$file in %s on line %d

Warning: Undefined property: Exception::$previous in %s on line %d

Warning: Undefined property: Exception::$previous in %s on line %d
string(41) "Exception in :1337
Stack trace:
#0 {main}"
