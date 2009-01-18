--TEST--
dbase_get_record_with_names() - Basic test
--FILE--
<?php

date_default_timezone_set("UTC");

$file = dirname(__FILE__) .'/dbase_get_record_with_names.dbf';

$def = array(
	array("date",     "D"),
	array("name",     "C",  50),
	array("email",    "C", 128),
	array("ismember", "L")
);


dbase_create($file, $def);

if ($db = dbase_open($file, 2)) {
	dbase_add_record($db, array(
		date('Ymd'),
		'Name #1',
		'Email #1',
		'T'));
	dbase_delete_record($db, 1);
	var_dump(dbase_get_record_with_names($db, 1));

	dbase_add_record($db, array(
		date('Ymd'),
		'Name #2',
		'Email #2',
		'T'));
	var_dump(dbase_get_record_with_names($db, 2));
	
	var_dump(dbase_numrecords($db));
	var_dump(dbase_numfields($db));
		
	dbase_close($db);
}

@unlink($file);

?>
--EXPECTF--
array(5) {
  ["date"]=>
  string(8) "%d"
  ["name"]=>
  string(50) "Name #1                                           "
  ["email"]=>
  string(128) "Email #1                                                                                                                        "
  ["ismember"]=>
  int(1)
  ["deleted"]=>
  int(1)
}
array(5) {
  ["date"]=>
  string(8) "%d"
  ["name"]=>
  string(50) "Name #2                                           "
  ["email"]=>
  string(128) "Email #2                                                                                                                        "
  ["ismember"]=>
  int(1)
  ["deleted"]=>
  int(0)
}
int(2)
int(4)
