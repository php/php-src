--TEST--
unserialize() turns C: objects of missing classes into incomplete objects
--FILE--
<?php

var_dump(unserialize('a:2:{i:0;C:7:"Missing":4:{abcd}i:1;s:4:"tail";}'));

function callback(string $class): void
{
    echo __FUNCTION__, "($class)\n";
}

ini_set('unserialize_callback_func', 'callback');

var_dump(unserialize('C:7:"Missing":4:{abcd}'));
?>
--EXPECTF--
Warning: Class __PHP_Incomplete_Class has no unserializer in %s on line %d
array(2) {
  [0]=>
  object(__PHP_Incomplete_Class)#%d (1) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(7) "Missing"
  }
  [1]=>
  string(4) "tail"
}
callback(Missing)

Warning: unserialize(): Function callback() hasn't defined the class it was called for in %s on line %d

Warning: Class __PHP_Incomplete_Class has no unserializer in %s on line %d
object(__PHP_Incomplete_Class)#%d (1) {
  ["__PHP_Incomplete_Class_Name"]=>
  string(7) "Missing"
}
