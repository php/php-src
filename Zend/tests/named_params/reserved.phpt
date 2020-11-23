--TEST--
Reserved keywords can be used with named parameters
--FILE--
<?php

function test($array) {
    var_dump($array);
}

test(array: []);

?>
--EXPECT--
array(0) {
}
