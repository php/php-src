--TEST--
Bug #76068 parse_ini_string fails to parse "[foo]\nbar=1|>baz" with segfault
--FILE--
<?php

$s = parse_ini_string("[foo]\nbar=1|>baz",true, \INI_SCANNER_TYPED);
var_dump($s);

$s = parse_ini_string("[foo]\nbar=\"1|>baz\"",true, \INI_SCANNER_TYPED);
var_dump($s);

$s = parse_ini_string("[foo]\nbar=1",true, \INI_SCANNER_TYPED);
var_dump($s);

$s = parse_ini_string("[foo]\nbar=42|>baz",true, \INI_SCANNER_TYPED);
var_dump($s);

?>
--EXPECT--
array(1) {
  ["foo"]=>
  array(1) {
    ["bar"]=>
    int(1)
  }
}
array(1) {
  ["foo"]=>
  array(1) {
    ["bar"]=>
    string(6) "1|>baz"
  }
}
array(1) {
  ["foo"]=>
  array(1) {
    ["bar"]=>
    int(1)
  }
}
array(1) {
  ["foo"]=>
  array(1) {
    ["bar"]=>
    int(42)
  }
}
