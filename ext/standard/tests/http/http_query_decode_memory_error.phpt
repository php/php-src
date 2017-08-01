--TEST--
http_query_decode() should not read uninitialized memory when checking for $this
--FILE--
<?php

function test() {
    // strlen("abcd") == 4 is relevant
    $array = http_query_decode('abcd=1');
    var_dump($array);
}

test();

?>
--EXPECT--
array(1) {
  ["abcd"]=>
  string(1) "1"
}
