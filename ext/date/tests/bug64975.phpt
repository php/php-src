--TEST--
Bug #64975: Wrong error parsing when AM/PM not at the end
--INI--
date.timezone=UTC
--FILE--
<?php
$date = DateTime::createFromFormat('d M Y A h:i', '11 Mar 2013 PM 3:34');
var_dump($date, DateTime::getLastErrors());
?>
--EXPECT--
bool(false)
array(4) {
  ["warning_count"]=>
  int(0)
  ["warnings"]=>
  array(0) {
  }
  ["error_count"]=>
  int(1)
  ["errors"]=>
  array(1) {
    [12]=>
    string(51) "Meridian can only come after an hour has been found"
  }
}
