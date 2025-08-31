--TEST--
Bug GH-9700 (DateTime::createFromFormat: Parsing TZID string is too greedy)
--FILE--
<?php
var_dump(DateTime::createFromFormat('Y-m-d\TH:i:sP[e]', '2022-02-18T00:00:00+01:00[Europe/Berlin]'));
?>
--EXPECTF--
object(DateTime)#%d (%d) {
  ["date"]=>
  string(26) "2022-02-18 00:00:00.000000"
  ["timezone_type"]=>
  int(3)
  ["timezone"]=>
  string(13) "Europe/Berlin"
}
