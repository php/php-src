--TEST--
foreach by ref followed by foreach by val
--FILE--
<?php

function test_basic(array $array) {
    foreach ($array as &$val) {
        var_dump($val);
    }
    foreach ($array as $val) {
        var_dump($val);
    }
    var_dump($array);
}

function test_goto(array $array) {
    foreach ($array as $i => &$val) {
        var_dump($val);
        if ($i == 1) {
            goto done;
        }
    }

done:
    foreach ($array as $val) {
        var_dump($val);
    }
    var_dump($array);
}

function test_break1(array $array) {
    foreach ($array as $i => &$val) {
        var_dump($val);
        if ($i == 1) {
            while (true) {
                break 2;
            }
        }
    }

    foreach ($array as $val) {
        var_dump($val);
    }
    var_dump($array);
}

function test_break2(array $array) {
    while (true) {
        foreach ($array as $i => &$val) {
            var_dump($val);
            if ($i == 1) {
                break 2;
            }
        }
    }

    foreach ($array as $val) {
        var_dump($val);
    }
    var_dump($array);
}

function test_complex(array $array) {
    // No unwrapping with complex target variable.
    $foo = [];
    foreach ($array as &$foo['val']) {
        var_dump($foo['val']);
    }
    foreach ($array as $foo['val']) {
        var_dump($foo['val']);
    }
    var_dump($array);
}

test_basic([1, 2, 3]);
echo "\n";
test_goto([1, 2, 3]);
echo "\n";
test_break1([1, 2, 3]);
echo "\n";
test_break2([1, 2, 3]);
echo "\n";
test_complex([1, 2, 3]);

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
int(2)
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  &int(2)
}
