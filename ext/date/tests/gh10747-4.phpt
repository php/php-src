--TEST--
Bug GH-10747 (Private fields in serialized DatePeriod objects throw)
--INI--
date.timezone=UTC
--FILE--
<?php
class I extends DatePeriod
{
	private   int $var1;
	private       $var2 = 2;
	protected int $var3 = 3;
	protected     $var4;

	function __construct($start, $interval, $end)
	{
		parent::__construct($start, $interval, $end);
		$this->var1 = 1;
		$this->var4 = 4;
	}
}

$i = new I(new DateTimeImmutable('2023-03-03 16:24'), DateInterval::createFromDateString('+1 hour'), new DateTimeImmutable('2023-03-09 16:24'));
$s = serialize($i);
$u = unserialize($s);

var_dump($i, str_replace(chr(0), '!', $s), $u);
?>
--EXPECT--
object(I)#1 (11) {
  ["start"]=>
  object(DateTimeImmutable)#5 (3) {
    ["date"]=>
    string(26) "2023-03-03 16:24:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(3) "UTC"
  }
  ["current"]=>
  NULL
  ["end"]=>
  object(DateTimeImmutable)#6 (3) {
    ["date"]=>
    string(26) "2023-03-09 16:24:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(3) "UTC"
  }
  ["interval"]=>
  object(DateInterval)#7 (11) {
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(0)
    ["h"]=>
    int(1)
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
    ["date_string"]=>
    NULL
  }
  ["recurrences"]=>
  int(1)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
  ["var1":"I":private]=>
  int(1)
  ["var2":"I":private]=>
  int(2)
  ["var3":protected]=>
  int(3)
  ["var4":protected]=>
  int(4)
}
string(652) "O:1:"I":11:{s:5:"start";O:17:"DateTimeImmutable":3:{s:4:"date";s:26:"2023-03-03 16:24:00.000000";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}s:7:"current";N;s:3:"end";O:17:"DateTimeImmutable":3:{s:4:"date";s:26:"2023-03-09 16:24:00.000000";s:13:"timezone_type";i:3;s:8:"timezone";s:3:"UTC";}s:8:"interval";O:12:"DateInterval":11:{s:1:"y";i:0;s:1:"m";i:0;s:1:"d";i:0;s:1:"h";i:1;s:1:"i";i:0;s:1:"s";i:0;s:1:"f";d:0;s:6:"invert";i:0;s:4:"days";b:0;s:11:"from_string";b:0;s:11:"date_string";N;}s:11:"recurrences";i:1;s:18:"include_start_date";b:1;s:16:"include_end_date";b:0;s:7:"!I!var1";i:1;s:7:"!I!var2";i:2;s:7:"!*!var3";i:3;s:7:"!*!var4";i:4;}"
object(I)#2 (11) {
  ["start"]=>
  object(DateTimeImmutable)#9 (3) {
    ["date"]=>
    string(26) "2023-03-03 16:24:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(3) "UTC"
  }
  ["current"]=>
  NULL
  ["end"]=>
  object(DateTimeImmutable)#10 (3) {
    ["date"]=>
    string(26) "2023-03-09 16:24:00.000000"
    ["timezone_type"]=>
    int(3)
    ["timezone"]=>
    string(3) "UTC"
  }
  ["interval"]=>
  object(DateInterval)#11 (11) {
    ["y"]=>
    int(0)
    ["m"]=>
    int(0)
    ["d"]=>
    int(0)
    ["h"]=>
    int(1)
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
    ["date_string"]=>
    NULL
  }
  ["recurrences"]=>
  int(1)
  ["include_start_date"]=>
  bool(true)
  ["include_end_date"]=>
  bool(false)
  ["var1":"I":private]=>
  int(1)
  ["var2":"I":private]=>
  int(2)
  ["var3":protected]=>
  int(3)
  ["var4":protected]=>
  int(4)
}
