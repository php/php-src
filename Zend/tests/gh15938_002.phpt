--TEST--
GH-15938
--FILE--
<?php

#[AllowDynamicProperties]
class C {}

$obj = new C();
$obj->a = '';
$obj->a .= new class {
    function __toString() {
        global $obj;
        for ($i = 0; $i < 8; $i++) {
            $obj->{$i} = 0;
        }
        return 'str';
    }
};

var_dump($obj);

?>
--EXPECTF--
object(C)#%d (9) {
  ["a"]=>
  string(3) "str"
  ["0"]=>
  int(0)
  ["1"]=>
  int(0)
  ["2"]=>
  int(0)
  ["3"]=>
  int(0)
  ["4"]=>
  int(0)
  ["5"]=>
  int(0)
  ["6"]=>
  int(0)
  ["7"]=>
  int(0)
}
