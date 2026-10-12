--TEST--
GH-15938
--FILE--
<?php

$a = [];
$a[0] = '';
$a[0] .= new class {
    function __toString() {
        global $a;
        for ($i = 0; $i < 8; $i++) {
            $a[] = 0;
        }
        return 'str';
    }
};

var_dump($a);

?>
--EXPECT--
array(9) {
  [0]=>
  string(3) "str"
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(0)
  [4]=>
  int(0)
  [5]=>
  int(0)
  [6]=>
  int(0)
  [7]=>
  int(0)
  [8]=>
  int(0)
}
