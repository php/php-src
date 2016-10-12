--TEST--
Bug #73091 (Unserializing DateInterval object may lead to __toString invocation)
--FILE--
<?php
class foo {
    function __toString() {
        var_dump(0);
        return 'may be a bug';
    }
}
    
var_dump(unserialize('O:12:"DateInterval":1:{s:4:"days";O:3:"foo":0:{}}'));
?>
--EXPECTF--
object(DateInterval)#%d (15) {
  ["days"]=>
  int(-1)
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
  ["special_type"]=>
  int(0)
  ["special_amount"]=>
  int(-1)
  ["have_weekday_relative"]=>
  int(0)
  ["have_special_relative"]=>
  int(0)
}
