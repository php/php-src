--TEST--
Regression introduce in fix for Bug #67118 - Invalid code
--INI--
date.timezone=Europe/Paris
--FILE--
<?php
class Foo extends DateTime {
    public function __construct($time = null) {
        $tz = new DateTimeZone('UTC');
        try {
            echo "First try\n";
            parent::__construct($time, $tz);
            return;
        } catch (Exception $e) {
            echo "Second try\n";
            parent::__construct($time.'C', $tz);
        }
    }
}
$date = '12 Sep 2007 15:49:12 UT';
var_dump(new Foo($date));
?>
Done
--EXPECT--
First try
Second try
object(Foo)#1 (3) {
  ["date"]=>
  string(26) "2007-09-12 15:49:12.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(3) "UTC"
}
Done
