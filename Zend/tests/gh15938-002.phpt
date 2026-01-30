--TEST--
GH-15938 002: ASSIGN_OBJ_OP: Properties ht may be resized by __toString(), write happens on freed buckets
--FILE--
<?php

#[AllowDynamicProperties]
class C {
    public $a;
}

$obj = new C();
$obj->b = '';

$obj->b .= new class {
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
==DONE==
--EXPECTF--
object(C)#%d (10) {
  ["a"]=>
  NULL
  ["b"]=>
  string(0) ""
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
==DONE==
