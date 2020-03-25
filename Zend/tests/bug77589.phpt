--TEST--
BUG #77589 (Core dump using parse_ini_string with numeric sections)
--FILE--
<?php
var_dump(
	parse_ini_string(<<<INI
[0]
a = 1
b = on
c = true

["true"]
a = 100 
b = null
c = yes
INI
, TRUE, INI_SCANNER_TYPED));

?>
--EXPECT--
array(2) {
  [0]=>
  array(3) {
    ["a"]=>
    int(1)
    ["b"]=>
    bool(true)
    ["c"]=>
    bool(true)
  }
  ["true"]=>
  array(3) {
    ["a"]=>
    int(100)
    ["b"]=>
    NULL
    ["c"]=>
    bool(true)
  }
}
