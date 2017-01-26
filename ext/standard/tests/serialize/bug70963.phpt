--TEST--
Bug #70963 (Unserialize shows UNKNOW in result)
--FILE--
<?php
var_dump(unserialize('a:2:{i:0;O:9:"exception":1:{s:16:"'."\0".'Exception'."\0".'trace";s:4:"test";}i:1;R:3;}'));
var_dump(unserialize('a:2:{i:0;O:9:"exception":1:{s:16:"'."\0".'Exception'."\0".'trace";s:4:"test";}i:1;r:3;}'));
?>
--EXPECTF--
array(2) {
  [0]=>
  object(Exception)#%d (6) {
    ["message":protected]=>
    string(0) ""
    ["string":"Exception":private]=>
    string(0) ""
    ["code":protected]=>
    int(0)
    ["file":protected]=>
    string(%d) "%s"
    ["line":protected]=>
    int(2)
    ["previous":"Exception":private]=>
    NULL
  }
  [1]=>
  string(4) "test"
}
array(2) {
  [0]=>
  object(Exception)#%d (6) {
    ["message":protected]=>
    string(0) ""
    ["string":"Exception":private]=>
    string(0) ""
    ["code":protected]=>
    int(0)
    ["file":protected]=>
    string(%d) "%s"
    ["line":protected]=>
    int(3)
    ["previous":"Exception":private]=>
    NULL
  }
  [1]=>
  string(4) "test"
}
