--TEST--
serialization: arrays with references to the containing array
--FILE--
<?php
function check(&$a) {
    var_dump($a);
    $ser = serialize($a);
    var_dump($ser);

    $b = unserialize($ser);
    var_dump($b);
    $b[0] = "b0.changed";
    var_dump($b);
    $b[1] = "b1.changed";
    var_dump($b);
    $b[2] = "b2.changed";
    var_dump($b);
}

echo "\n\n--- 1 refs container:\n";
$a = array();
$a[0] = &$a;
$a[1] = 1;
$a[2] = 1;
check($a);

echo "\n\n--- 1,2 ref container:\n";
$a = array();
$a[0] = &$a;
$a[1] = &$a;
$a[2] = 1;
check($a);

echo "\n\n--- 1,2,3 ref container:\n";
$a = array();
$a[0] = &$a;
$a[1] = &$a;
$a[2] = &$a;
check($a);

echo "Done";
?>
--EXPECT--
--- 1 refs container:
array(3) {
  [0]=>
  *RECURSION*
  [1]=>
  int(1)
  [2]=>
  int(1)
}
string(56) "a:3:{i:0;a:3:{i:0;R:2;i:1;i:1;i:2;i:1;}i:1;i:1;i:2;i:1;}"
array(3) {
  [0]=>
  &array(3) {
    [0]=>
    *RECURSION*
    [1]=>
    int(1)
    [2]=>
    int(1)
  }
  [1]=>
  int(1)
  [2]=>
  int(1)
}
array(3) {
  [0]=>
  string(10) "b0.changed"
  [1]=>
  int(1)
  [2]=>
  int(1)
}
array(3) {
  [0]=>
  string(10) "b0.changed"
  [1]=>
  string(10) "b1.changed"
  [2]=>
  int(1)
}
array(3) {
  [0]=>
  string(10) "b0.changed"
  [1]=>
  string(10) "b1.changed"
  [2]=>
  string(10) "b2.changed"
}


--- 1,2 ref container:
array(3) {
  [0]=>
  *RECURSION*
  [1]=>
  *RECURSION*
  [2]=>
  int(1)
}
string(56) "a:3:{i:0;a:3:{i:0;R:2;i:1;R:2;i:2;i:1;}i:1;R:2;i:2;i:1;}"
array(3) {
  [0]=>
  &array(3) {
    [0]=>
    *RECURSION*
    [1]=>
    *RECURSION*
    [2]=>
    int(1)
  }
  [1]=>
  &array(3) {
    [0]=>
    *RECURSION*
    [1]=>
    *RECURSION*
    [2]=>
    int(1)
  }
  [2]=>
  int(1)
}
array(3) {
  [0]=>
  &string(10) "b0.changed"
  [1]=>
  &string(10) "b0.changed"
  [2]=>
  int(1)
}
array(3) {
  [0]=>
  &string(10) "b1.changed"
  [1]=>
  &string(10) "b1.changed"
  [2]=>
  int(1)
}
array(3) {
  [0]=>
  &string(10) "b1.changed"
  [1]=>
  &string(10) "b1.changed"
  [2]=>
  string(10) "b2.changed"
}


--- 1,2,3 ref container:
array(3) {
  [0]=>
  *RECURSION*
  [1]=>
  *RECURSION*
  [2]=>
  *RECURSION*
}
string(56) "a:3:{i:0;a:3:{i:0;R:2;i:1;R:2;i:2;R:2;}i:1;R:2;i:2;R:2;}"
array(3) {
  [0]=>
  &array(3) {
    [0]=>
    *RECURSION*
    [1]=>
    *RECURSION*
    [2]=>
    *RECURSION*
  }
  [1]=>
  &array(3) {
    [0]=>
    *RECURSION*
    [1]=>
    *RECURSION*
    [2]=>
    *RECURSION*
  }
  [2]=>
  &array(3) {
    [0]=>
    *RECURSION*
    [1]=>
    *RECURSION*
    [2]=>
    *RECURSION*
  }
}
array(3) {
  [0]=>
  &string(10) "b0.changed"
  [1]=>
  &string(10) "b0.changed"
  [2]=>
  &string(10) "b0.changed"
}
array(3) {
  [0]=>
  &string(10) "b1.changed"
  [1]=>
  &string(10) "b1.changed"
  [2]=>
  &string(10) "b1.changed"
}
array(3) {
  [0]=>
  &string(10) "b2.changed"
  [1]=>
  &string(10) "b2.changed"
  [2]=>
  &string(10) "b2.changed"
}
Done
