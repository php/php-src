--TEST--
parse_str() should not read uninitialized memory when checking for $this
--FILE--
<?php

function test() {
    // strlen("abcd") == 4 is relevant
    parse_str('abcd=1', $array);
    var_dump($array);
}

test();

?>
--EXPECT--
array(1) {
  ["abcd"]=>
  string(1) "1"
}
