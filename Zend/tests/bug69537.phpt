--TEST--
Bug #69537 (__debugInfo with empty string for key gives error)
--FILE--
<?php
class Foo {

    public function __debugInfo(){
        return ['' => 1];
    }
}

var_dump(new Foo);
?>
--EXPECTF--
object(Foo)#%d (%d) {
  [""]=>
  int(1)
}

