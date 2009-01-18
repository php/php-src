--TEST--
dbase_pack() - Basic test
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
	dbase_add_record($db, array(
		date('Ymd'),
		'Name #1',
		'Email #1',
		'T'));
	dbase_delete_record($db, 1);
	var_dump(dbase_get_record($db, 1));
	
	dbase_pack($db);
	
	var_dump(dbase_get_record($db, 1));
		
	dbase_close($db);
}

@unlink($file);

?>
--EXPECTF--
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
  int(1)
}

Warning: dbase_get_record(): Tried to read bad record 1 in %s on line %d
bool(false)
