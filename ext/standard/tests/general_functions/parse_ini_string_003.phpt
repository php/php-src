--TEST--
parse_ini_string() typed scanner mode
--FILE--
<?php

$contents = <<<EOS
foo = 1
bar = 1.3
baz = null
qux[] = false
qux[] = off
qux[] = something
qux[] = "something else"
EOS;

var_dump(parse_ini_string($contents, false, INI_SCANNER_TYPED));

?>
Done
--EXPECTF--
array(4) {
  ["foo"]=>
  int(1)
  ["bar"]=>
  float(1.3)
  ["baz"]=>
  NULL
  ["qux"]=>
  array(4) {
    [0]=>
    bool(false)
    [1]=>
    bool(false)
    [2]=>
    string(9) "something"
    [3]=>
    string(14) "something else"
  }
}
Done
