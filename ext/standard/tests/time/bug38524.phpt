--TEST--
Bug #38524 (strptime() does not initialize the internal date storage structure)
--SKIPIF--
<?php if (!function_exists('strptime')) echo "SKIP"; ?>
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
