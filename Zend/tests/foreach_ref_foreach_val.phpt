--TEST--
foreach by ref followed by foreach by val
--FILE--
<?php

function test1(array $array) {
    foreach ($array as &$val) {
        var_dump($val);
    }
    foreach ($array as $val) {
        var_dump($val);
    }
    var_dump($array);
}

function test2(array $array) {
    $foo = [];
    foreach ($array as &$foo['val']) {
        var_dump($foo['val']);
    }
    foreach ($array as $foo['val']) {
        var_dump($foo['val']);
    }
    var_dump($array);
}

test1([1, 2, 3]);
echo "\n";
test2([1, 2, 3]);

?>
--EXPECT--
int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

int(1)
int(2)
int(3)
int(1)
int(2)
int(3)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
