--TEST--
Test localtime() function : usage variation - Passing hexa decimal values to timestamp.
--FILE--
<?php
echo "*** Testing localtime() : usage variation ***\n";

date_default_timezone_set("UTC");
// Initialise function arguments not being substituted (if any)
$is_associative = true;

//array of values to iterate over
$inputs = array(

      'Hexa-decimal 0' => 0x0,
      'Hexa-decimal 10' => 0xA,
      'Hexa-decimal -10' => -0XA
);

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( localtime($value) );
      var_dump( localtime($value, $is_associative) );
}

?>
--EXPECT--
*** Testing localtime() : usage variation ***

--Hexa-decimal 0--
array(9) {
  [0]=>
  int(0)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(1)
  [4]=>
  int(0)
  [5]=>
  int(70)
  [6]=>
  int(4)
  [7]=>
  int(0)
  [8]=>
  int(0)
}
array(9) {
  ["tm_sec"]=>
  int(0)
  ["tm_min"]=>
  int(0)
  ["tm_hour"]=>
  int(0)
  ["tm_mday"]=>
  int(1)
  ["tm_mon"]=>
  int(0)
  ["tm_year"]=>
  int(70)
  ["tm_wday"]=>
  int(4)
  ["tm_yday"]=>
  int(0)
  ["tm_isdst"]=>
  int(0)
}

--Hexa-decimal 10--
array(9) {
  [0]=>
  int(10)
  [1]=>
  int(0)
  [2]=>
  int(0)
  [3]=>
  int(1)
  [4]=>
  int(0)
  [5]=>
  int(70)
  [6]=>
  int(4)
  [7]=>
  int(0)
  [8]=>
  int(0)
}
array(9) {
  ["tm_sec"]=>
  int(10)
  ["tm_min"]=>
  int(0)
  ["tm_hour"]=>
  int(0)
  ["tm_mday"]=>
  int(1)
  ["tm_mon"]=>
  int(0)
  ["tm_year"]=>
  int(70)
  ["tm_wday"]=>
  int(4)
  ["tm_yday"]=>
  int(0)
  ["tm_isdst"]=>
  int(0)
}

--Hexa-decimal -10--
array(9) {
  [0]=>
  int(50)
  [1]=>
  int(59)
  [2]=>
  int(23)
  [3]=>
  int(31)
  [4]=>
  int(11)
  [5]=>
  int(69)
  [6]=>
  int(3)
  [7]=>
  int(364)
  [8]=>
  int(0)
}
array(9) {
  ["tm_sec"]=>
  int(50)
  ["tm_min"]=>
  int(59)
  ["tm_hour"]=>
  int(23)
  ["tm_mday"]=>
  int(31)
  ["tm_mon"]=>
  int(11)
  ["tm_year"]=>
  int(69)
  ["tm_wday"]=>
  int(3)
  ["tm_yday"]=>
  int(364)
  ["tm_isdst"]=>
  int(0)
}
