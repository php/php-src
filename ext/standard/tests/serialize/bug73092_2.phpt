--TEST--
Bug #73092: Unserialize use-after-free when resizing object's properties hash table (2)
--FILE--
<?php
class foo {
    public $x;
    function __wakeup() {
        var_dump($this->x);
    }
}

unserialize('a:3:{i:0;O:3:"foo":1:{s:1:"x";O:12:"DateInterval":1:{i:0;i:0;}}i:1;s:263:"'.str_repeat("\06", 263).'";i:2;r:4;}}');

?>
--EXPECTF--
object(DateInterval)#%d (16) {
  ["0"]=>
  int(0)
  ["y"]=>
  int(-1)
  ["m"]=>
  int(-1)
  ["d"]=>
  int(-1)
  ["h"]=>
  int(-1)
  ["i"]=>
  int(-1)
  ["s"]=>
  int(-1)
  ["weekday"]=>
  int(-1)
  ["weekday_behavior"]=>
  int(-1)
  ["first_last_day_of"]=>
  int(-1)
  ["invert"]=>
  int(0)
  ["days"]=>
  int(-1)
  ["special_type"]=>
  int(0)
  ["special_amount"]=>
  int(-1)
  ["have_weekday_relative"]=>
  int(0)
  ["have_special_relative"]=>
  int(0)
}
