--TEST--
Test getdate() function : usage variation - Passing strings containing numbers
--FILE--
<?php
echo "*** Testing getdate() : usage variation ***\n";

date_default_timezone_set("Asia/Calcutta");

//Timezones with required data for date_sunrise
$inputs = array (
        'String 0' => '0',
        'String 10.5' => "10.5",
        'String -10.5' => '-10.5',
);

// loop through each element of the array for timestamp
foreach($inputs as $key => $value) {
      echo "\n--$key--\n";
      var_dump( getdate($value) );
};
?>
--EXPECT--
*** Testing getdate() : usage variation ***

--String 0--
array(11) {
  ["seconds"]=>
  int(0)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(0)
}

--String 10.5--
array(11) {
  ["seconds"]=>
  int(10)
  ["minutes"]=>
  int(30)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(10)
}

--String -10.5--
array(11) {
  ["seconds"]=>
  int(50)
  ["minutes"]=>
  int(29)
  ["hours"]=>
  int(5)
  ["mday"]=>
  int(1)
  ["wday"]=>
  int(4)
  ["mon"]=>
  int(1)
  ["year"]=>
  int(1970)
  ["yday"]=>
  int(0)
  ["weekday"]=>
  string(8) "Thursday"
  ["month"]=>
  string(7) "January"
  [0]=>
  int(-10)
}
