--TEST--
Bug #77691: Opcache passes wrong value for inline array push assignments
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

if (true) {
    function dump($str) {
        var_dump($str);
    }
}

function test() {
    $array = [];
    dump($array[] = 'test');
    dump($array);
}

test();

?>
--EXPECT--
string(4) "test"
array(1) {
  [0]=>
  string(4) "test"
}
