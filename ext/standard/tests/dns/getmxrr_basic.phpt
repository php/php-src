--TEST--
getmxrr() basic usage
--SKIPIF--
<?php require "skipif.inc"; ?>
--FILE--
<?php
var_dump(getmxrr('basic.dnstest.php.net', $hosts, $weights));
var_dump($hosts, $weights);

var_dump(getmxrr('www.basic.dnstest.php.net', $hosts, $weights));
var_dump($hosts, $weights);
?>
--EXPECT--
bool(true)
array(1) {
  [0]=>
  string(25) "mx1.basic.dnstest.php.net"
}
array(1) {
  [0]=>
  int(10)
}
bool(false)
array(0) {
}
array(0) {
}
