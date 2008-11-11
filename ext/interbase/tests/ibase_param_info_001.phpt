--TEST--
ibase_param_info(): Basic test
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

require("interbase.inc");

$x = ibase_connect($test_base);

$rs = ibase_prepare('SELECT * FROM test1 WHERE 1 = ? AND 2 = ?');
var_dump(ibase_param_info($rs, 1));

print "---\n";

var_dump(ibase_param_info($rs, 100));

print "---\n";

var_dump(ibase_param_info(100));


?>
--EXPECTF--
array(10) {
  [0]=>
  string(0) ""
  ["name"]=>
  string(0) ""
  [1]=>
  string(0) ""
  ["alias"]=>
  string(0) ""
  [2]=>
  string(0) ""
  ["relation"]=>
  string(0) ""
  [3]=>
  string(1) "4"
  ["length"]=>
  string(1) "4"
  [4]=>
  string(7) "INTEGER"
  ["type"]=>
  string(7) "INTEGER"
}
---
bool(false)
---

Warning: Wrong parameter count for ibase_param_info() in %s on line %d
NULL

Warning: Unknown: invalid statement handle  in Unknown on line 0
