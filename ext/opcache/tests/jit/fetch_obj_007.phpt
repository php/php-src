--TEST--
JIT: FETCH_OBJ 007
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
--FILE--
<?php
class C {
    public ?C $prop = null;
}
function foo($obj) {
    $obj->prop->prop = null;
}

$obj = new C;
$obj->prop = new C;
for ($i = 0; $i < 10; $i++) {
    foo($obj);
}
var_dump($obj);
?>
--EXPECT--
object(C)#1 (1) {
  ["prop"]=>
  object(C)#2 (1) {
    ["prop"]=>
    NULL
  }
}
