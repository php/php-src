--TEST--
Cookies test#2
--COOKIE--
c o o k i e=value; c o o k i e= v a l u e ;;c%20o+o k+i%20e=v;name="value","value",UEhQIQ==;UEhQIQ==foo
--FILE--
<?php
var_dump($_COOKIE);
?>
--EXPECT--
array(3) {
  [u"c_o_o_k_i_e"]=>
  unicode(1) "v"
  [u"name"]=>
  unicode(24) ""value","value",UEhQIQ=="
  [u"UEhQIQ"]=>
  unicode(4) "=foo"
}
