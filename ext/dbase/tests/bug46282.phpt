--TEST--
Bug #46282 (Corrupt DBF When Using DATE)
--FILE--
<?php

date_default_timezone_set("UTC");

$def = array(
	array("date",     "D"),
	array("name",     "C",  50),
	array("email",    "C", 128),
	array("ismember", "L")
);

$file = dirname(__FILE__) .'/bug46282test.dbf';

// creation
if (!dbase_create($file, $def)) {
	echo "Error, can't create the database\n";
}

// open in read-write mode
$db = dbase_open($file, 2);

if ($db) {
	for ($i = 0; $i < 5; $i++) {
		dbase_add_record($db, array(
			date('Ymd'),
			'Name #'. $i,
			'Email #'. $i,
			'T'));
		var_dump(dbase_get_record($db, $i+1));
	}
}
dbase_close($db);

@unlink($file);

?>
--EXPECTF--
array(5) {
  [0]=>
  string(8) "%d"
  [1]=>
  string(50) "Name #0                                           "
  [2]=>
  string(128) "Email #0                                                                                                                        "
  [3]=>
  int(1)
  ["deleted"]=>
  int(0)
}
array(5) {
  [0]=>
  string(8) "%d"
  [1]=>
  string(50) "Name #1                                           "
  [2]=>
  string(128) "Email #1                                                                                                                        "
  [3]=>
  int(1)
  ["deleted"]=>
  int(0)
}
array(5) {
  [0]=>
  string(8) "%d"
  [1]=>
  string(50) "Name #2                                           "
  [2]=>
  string(128) "Email #2                                                                                                                        "
  [3]=>
  int(1)
  ["deleted"]=>
  int(0)
}
array(5) {
  [0]=>
  string(8) "%d"
  [1]=>
  string(50) "Name #3                                           "
  [2]=>
  string(128) "Email #3                                                                                                                        "
  [3]=>
  int(1)
  ["deleted"]=>
  int(0)
}
array(5) {
  [0]=>
  string(8) "%d"
  [1]=>
  string(50) "Name #4                                           "
  [2]=>
  string(128) "Email #4                                                                                                                        "
  [3]=>
  int(1)
  ["deleted"]=>
  int(0)
}
