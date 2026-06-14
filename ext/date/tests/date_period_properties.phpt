--TEST--
Test different usages of DatePeriod properties
--FILE--
<?php

class MyDatePeriod extends DatePeriod {
    public int $prop = 3;
}

$period = MyDatePeriod::createFromISO8601String("R4/2012-07-01T00:00:00Z/P7D");

var_dump($period);
var_dump(json_encode($period));
var_dump(serialize($period));
var_dump(get_object_vars($period));
var_dump(var_export($period));
var_dump((array) ($period));

?>
--EXPECTF--
object(MyDatePeriod)#%d (%d) {
  ["prop"]=>
  int(3)
  ["start"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "2012-07-01 00:00:00.000000"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
  ["current"]=>
  NULL
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateInterval)#%d (%d) {
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(7)
    ["h"]=>
    int(0)
    ["i"]=>
    int(0)
    ["s"]=>
    int(0)
    ["f"]=>
    float(0)
    ["invert"]=>
    int(0)
    ["days"]=>
    bool(false)
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
}
string(%d) "{"prop":3,"start":{"date":"2012-07-01 00:00:00.000000","timezone_type":1,"timezone":"+00:00"},"current":null,"end":null,"interval":{"y":0,"m":0,"d":7,"h":0,"i":0,"s":0,"f":0,"invert":0,"days":false,"from_string":false},"recurrences":5,"include_start_date":true,"include_end_date":false}"
string(%d) "O:12:"MyDatePeriod":8:{s:5:"start";O:17:"DateTimeImmutable":3:{s:4:"date";s:26:"2012-07-01 00:00:00.000000";s:13:"timezone_type";i:1;s:8:"timezone";s:6:"+00:00";}s:7:"current";N;s:3:"end";N;s:8:"interval";O:12:"DateInterval":10:{s:1:"y";i:0;s:1:"m";i:0;s:1:"d";i:7;s:1:"h";i:0;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:6:"invert";i:0;s:4:"days";b:0;s:11:"from_string";b:0;}s:11:"recurrences";i:5;s:18:"include_start_date";b:1;s:16:"include_end_date";b:0;s:4:"prop";i:3;}"
array(%d) {
  ["prop"]=>
  int(3)
  ["start"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "2012-07-01 00:00:00.000000"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
  ["current"]=>
  NULL
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateInterval)#%d (%d) {
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(7)
    ["h"]=>
    int(0)
    ["i"]=>
    int(0)
    ["s"]=>
    int(0)
    ["f"]=>
    float(0)
    ["invert"]=>
    int(0)
    ["days"]=>
    bool(false)
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
}
\MyDatePeriod::__set_state(array(
   'prop' => 3,
   'start' => 
  \DateTimeImmutable::__set_state(array(
     'date' => '2012-07-01 00:00:00.000000',
     'timezone_type' => 1,
     'timezone' => '+00:00',
  )),
   'current' => NULL,
   'end' => NULL,
   'interval' => 
  \DateInterval::__set_state(array(
     'y' => 0,
     'm' => 0,
     'd' => 7,
     'h' => 0,
     'i' => 0,
     's' => 0,
     'f' => 0.0,
     'invert' => 0,
     'days' => false,
     'from_string' => false,
  )),
   'recurrences' => 5,
   'include_start_date' => true,
   'include_end_date' => false,
))NULL
array(%d) {
  ["prop"]=>
  int(3)
  ["start"]=>
  object(DateTimeImmutable)#%d (%d) {
    ["date"]=>
    string(26) "2012-07-01 00:00:00.000000"
    ["timezone_type"]=>
    int(1)
    ["timezone"]=>
    string(6) "+00:00"
  }
  ["current"]=>
  NULL
  ["end"]=>
  NULL
  ["interval"]=>
  object(DateInterval)#%d (%d) {
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(7)
    ["h"]=>
    int(0)
    ["i"]=>
    int(0)
    ["s"]=>
    int(0)
    ["f"]=>
    float(0)
    ["invert"]=>
    int(0)
    ["days"]=>
    bool(false)
    ["from_string"]=>
    bool(false)
  }
  ["recurrences"]=>
  int(5)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
}
