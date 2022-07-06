--TEST--
parse_ini_string() crashes on values starting with number or unquoted strings
--FILE--
<?php

$contents = <<<EOS
[agatha.christie]
title = 10 little indians
foo[123] = E_ALL & ~E_DEPRECATED
foo[456] = 123
EOS;

var_dump(parse_ini_string($contents, false, INI_SCANNER_TYPED));

?>
Done
--EXPECTF--
array(%d) {
  ["title"]=>
  string(%d) "10 little indians"
  ["foo"]=>
  array(%d) {
    [123]=>
    string(%d) "24575"
    [456]=>
    int(123)
  }
}
Done
