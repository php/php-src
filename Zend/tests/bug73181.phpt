--TEST--
Bug #73181: parse_str() without a second argument leads to crash
--FILE--
<?php

function x() {
    parse_str("1&x");
    var_dump(get_defined_vars());
}

x();

?>
--EXPECT--
array(2) {
  [1]=>
  string(0) ""
  ["x"]=>
  string(0) ""
}
