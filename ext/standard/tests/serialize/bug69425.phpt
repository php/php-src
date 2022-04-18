--TEST--
Bug #69425: Use After Free in unserialize()
--FILE--
<?php

// POC 1
class test
{
    var $ryat;

    function __wakeup()
    {
        $this->ryat = 1;
    }
}

$data = unserialize('a:2:{i:0;O:4:"test":1:{s:4:"ryat";R:1;}i:1;i:2;}');
var_dump($data);

// POC 2
$data = unserialize('a:2:{i:0;O:12:"DateInterval":1:{s:1:"y";R:1;}i:1;i:2;}');
var_dump($data);

?>
--EXPECT--
int(1)
array(2) {
  [0]=>
  object(DateInterval)#1 (16) {
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
    ["f"]=>
    float(0)
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
  [1]=>
  int(2)
}
