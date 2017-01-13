--TEST--
Bug #70947 (INI parser segfault with INI_SCANNER_TYPED)
--FILE--
<?php
$o = parse_ini_string('foo = bar 123', FALSE, INI_SCANNER_TYPED);
var_dump($o);
?>
--EXPECT--
array(1) {
  ["foo"]=>
  string(7) "bar 123"
}
