--TEST--
Bug #70213: Unserialize context shared on double class lookup
--FILE--
<?php

ini_set('unserialize_callback_func', 'evil');

function evil() {
	spl_autoload_register(function ($arg) {
        var_dump(unserialize('R:1;'));
    });
}

var_dump(unserialize('a:2:{i:0;i:42;i:1;O:4:"evil":0:{}}'));

?>
--EXPECTF--
Notice: unserialize(): Error at offset 4 of 4 bytes in %s on line %d
bool(false)

Warning: unserialize(): Function evil() hasn't defined the class it was called for in %s on line %d
array(2) {
  [0]=>
  int(42)
  [1]=>
  object(__PHP_Incomplete_Class)#2 (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(4) "evil"
  }
}
