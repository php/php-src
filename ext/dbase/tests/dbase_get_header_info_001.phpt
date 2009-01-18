--TEST--
dbase_get_header_info() - Basic test
--FILE--
<?php

date_default_timezone_set("UTC");

$file = dirname(__FILE__) .'/dbase_get_header_info_001.dbf';

$def = array(
	array("date",     "D"),
	array("name",     "C",  50),
	array("email",    "C", 128),
	array("ismember", "L")
);


dbase_create($file, $def);

if ($db = dbase_open($file, 2)) {
	var_dump(dbase_get_header_info($db));
	dbase_close($db);
}

@unlink($file);

?>
--EXPECT--
array(4) {
  [0]=>
  array(6) {
    ["name"]=>
    string(4) "date"
    ["type"]=>
    string(4) "date"
    ["length"]=>
    int(8)
    ["precision"]=>
    int(0)
    ["format"]=>
    string(3) "%8s"
    ["offset"]=>
    int(1)
  }
  [1]=>
  array(6) {
    ["name"]=>
    string(4) "name"
    ["type"]=>
    string(9) "character"
    ["length"]=>
    int(50)
    ["precision"]=>
    int(0)
    ["format"]=>
    string(5) "%-50s"
    ["offset"]=>
    int(9)
  }
  [2]=>
  array(6) {
    ["name"]=>
    string(5) "email"
    ["type"]=>
    string(9) "character"
    ["length"]=>
    int(128)
    ["precision"]=>
    int(0)
    ["format"]=>
    string(6) "%-128s"
    ["offset"]=>
    int(59)
  }
  [3]=>
  array(6) {
    ["name"]=>
    string(8) "ismember"
    ["type"]=>
    string(7) "boolean"
    ["length"]=>
    int(1)
    ["precision"]=>
    int(0)
    ["format"]=>
    string(3) "%1s"
    ["offset"]=>
    int(187)
  }
}
