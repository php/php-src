--TEST--
PDO_PGSQL: PDO Unserializing
--SKIPIF--
<?php # vim:ft=php
require_once('skipif.inc');
?>
--FILE--
<?php

require_once('connection.inc');
require_once('prepare.inc');

require_once($PDO_TESTS . 'pdo_018.inc');

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
===INIT===
int(1)
int(1)
int(1)
string(1) "3"
array(3) {
  [0]=>
  string(8) "stdClass"
  [1]=>
  string(8) "TestBase"
  [2]=>
  string(11) "TestDerived"
}
===TYPES===
array(4) {
  ["stdClass"]=>
  int(0)
  ["TestBase"]=>
  int(1)
  ["TestDerived"]=>
  int(2)
  ["TestLeaf"]=>
  NULL
}
===INSERT===
TestBase::serialize() = 'a:3:{s:7:"BasePub";s:6:"Public";s:7:"BasePro";s:9:"Protected";s:7:"BasePri";s:7:"Private";}'
TestDerived::serialize()
TestBase::serialize() = 'a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}'
TestDerived::serialize()
TestBase::serialize() = 'a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}'
===DATA===
array(4) {
  [0]=>
  NULL
  [1]=>
  string(91) "a:3:{s:7:"BasePub";s:6:"Public";s:7:"BasePro";s:9:"Protected";s:7:"BasePri";s:7:"Private";}"
  [2]=>
  string(144) "a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}"
  [3]=>
  string(144) "a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}"
}
===FAILURE===
Exception:Class stdClass cannot be unserialized
===COUNT===
int(3)
===DATABASE===
array(3) {
  [0]=>
  array(2) {
    ["name"]=>
    string(8) "TestBase"
    ["val"]=>
    string(91) "a:3:{s:7:"BasePub";s:6:"Public";s:7:"BasePro";s:9:"Protected";s:7:"BasePri";s:7:"Private";}"
  }
  [1]=>
  array(2) {
    ["name"]=>
    string(11) "TestDerived"
    ["val"]=>
    string(144) "a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}"
  }
  [2]=>
  array(2) {
    ["name"]=>
    NULL
    ["val"]=>
    string(144) "a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";}"
  }
}
===FETCHCLASS===
TestBase::unserialize[22](a:3:{s:7:"BasePub";s:6:"Public";s:7:"BasePro";s:9:"Protected";s:7:"BasePri";s:7:"Private";})
TestDerived::unserialize()
TestBase::unserialize[22](a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";})
TestDerived::unserialize()
TestBase::unserialize[22](a:4:{s:7:"BasePub";s:13:"DerivedPublic";s:7:"BasePro";s:16:"DerivdeProtected";s:10:"DerivedPub";s:6:"Public";s:10:"DerivedPro";s:9:"Protected";})
array(3) {
  [0]=>
  object(TestBase)#%d (3) {
    ["BasePub"]=>
    string(7) "#Public"
    ["BasePro:protected"]=>
    string(10) "#Protected"
    ["BasePri:private"]=>
    string(8) "#Private"
  }
  [1]=>
  object(TestDerived)#%d (6) {
    ["BasePub"]=>
    string(14) "#DerivedPublic"
    ["BasePro:protected"]=>
    string(17) "#DerivdeProtected"
    ["DerivedPub"]=>
    string(7) "#Public"
    ["DerivedPro:protected"]=>
    string(10) "#Protected"
    ["DerivedPri:private"]=>
    string(7) "Private"
    ["BasePri:private"]=>
    string(7) "Private"
  }
  [2]=>
  object(TestLeaf)#%d (6) {
    ["BasePub"]=>
    string(14) "#DerivedPublic"
    ["BasePro:protected"]=>
    string(17) "#DerivdeProtected"
    ["DerivedPub"]=>
    string(7) "#Public"
    ["DerivedPro:protected"]=>
    string(10) "#Protected"
    ["DerivedPri:private"]=>
    string(7) "Private"
    ["BasePri:private"]=>
    string(7) "Private"
  }
}
===DONE===
