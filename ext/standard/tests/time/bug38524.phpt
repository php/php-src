--TEST--
Bug #38524 (strptime() does not initialize the internal date storage structure)
--FILE--
<?php
	var_dump(strptime('2006-08-20', '%Y-%m-%d'));
?>
===DONE===
--EXPECTF--
array(9) {
  ["tm_sec"]=>
  int(0)
  ["tm_min"]=>
  int(0)
  ["tm_hour"]=>
  int(0)
  ["tm_mday"]=>
  int(20)
  ["tm_mon"]=>
  int(7)
  ["tm_year"]=>
  int(106)
  ["tm_wday"]=>
  int(0)
  ["tm_yday"]=>
  int(%d)
  ["unparsed"]=>
  string(0) ""
}
===DONE===
--UEXPECTF--
array(9) {
  [u"tm_sec"]=>
  int(0)
  [u"tm_min"]=>
  int(0)
  [u"tm_hour"]=>
  int(0)
  [u"tm_mday"]=>
  int(20)
  [u"tm_mon"]=>
  int(7)
  [u"tm_year"]=>
  int(106)
  [u"tm_wday"]=>
  int(0)
  [u"tm_yday"]=>
  int(231)
  [u"unparsed"]=>
  string(0) ""
}
===DONE===
