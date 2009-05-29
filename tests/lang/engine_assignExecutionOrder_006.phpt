--TEST--
Evaluation order during assignments.
--FILE--
<?php

function i1() {
        echo "i1\n";
        return 0;
}

function i2() {
        echo "i2\n";
        return 0;
}

function i3() {
        echo "i3\n";
        return 0;
}

function i4() {
        echo "i4\n";
        return 0;
}

function i5() {
        echo "i5\n";
        return 0;
}

function i6() {
        echo "i6\n";
        return 0;
}

$a = array(array(0));
$b = array(array(1));
$c = array(array(2));

$a[i1()][i2()] = ($b[i3()][i4()] = $c[i5()][i6()]);
var_dump($a);
var_dump($b);
var_dump($c);

$a[i1()][i2()] = $b[i3()][i4()] = -$c[i5()][i6()];
var_dump($a);
var_dump($b);
var_dump($c);

$a[i1()][i2()] = -($b[i3()][i4()] = +($c[i5()][i6()]));
var_dump($a);
var_dump($b);
var_dump($c);


?>
--EXPECT--
i1
i2
i3
i4
i5
i6
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
}
i1
i2
i3
i4
i5
i6
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(-2)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(-2)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
}
i1
i2
i3
i4
i5
i6
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(-2)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(2)
  }
}
