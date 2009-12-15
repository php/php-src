--TEST--
Bug #50392 date_create_from_format enforces 6 digits for 'u' format character
--FILE--
<?php
date_default_timezone_set('Europe/Bratislava');

$base = '2009-03-01 18:00:00';

for ($i = 0; $i < 8; $i++) {
	var_dump(date_create_from_format('Y-m-d H:i:s.u', $base . '.' . str_repeat('1', $i)));
}
?>
--EXPECT--
bool(false)
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2009-03-01 18:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(17) "Europe/Bratislava"
}
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2009-03-01 18:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(17) "Europe/Bratislava"
}
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2009-03-01 18:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(17) "Europe/Bratislava"
}
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2009-03-01 18:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(17) "Europe/Bratislava"
}
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2009-03-01 18:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(17) "Europe/Bratislava"
}
object(DateTime)#2 (3) {
  ["date"]=>
  string(19) "2009-03-01 18:00:00"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(17) "Europe/Bratislava"
}
bool(false)
