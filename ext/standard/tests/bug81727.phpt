--TEST--
Bug #81727: $_COOKIE name starting with ..Host/..Secure should be discarded
--COOKIE--
..Host-test=ignore; __Host-test=correct; . Secure-test=ignore; . Elephpant=Awesome;
--FILE--
<?php
var_dump($_COOKIE);
?>
--EXPECT--
array(2) {
  ["__Host-test"]=>
  string(7) "correct"
  ["__Elephpant"]=>
  string(7) "Awesome"
}
