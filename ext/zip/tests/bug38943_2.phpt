--TEST--
#38943, properties in extended class cannot be set (5.3)
--SKIPIF--
<?php
/* $Id: bug38943_2.phpt 271800 2008-12-24 11:28:25Z pajoye $ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
include 'bug38943.inc';
?>
--EXPECTF--
array(1) {
  [0]=>
  int(1)
}
object(myZip)#1 (%d) {
  ["test":"myZip":private]=>
  int(0)
  ["testp"]=>
  string(6) "foobar"
  ["testarray":"myZip":private]=>
  array(1) {
    [0]=>
    int(1)
  }
  ["status"]=>
  int(0)
  ["statusSys"]=>
  int(0)
  ["numFiles"]=>
  int(0)
  ["filename"]=>
  string(0) ""
  ["comment"]=>
  string(0) ""
}
