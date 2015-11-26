--TEST--
Bug #45373 (php crash on query with errors in params)
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

	require("interbase.inc");
    
	$db = ibase_connect($test_base);


	$sql = "select * from test1 where i = ? and c = ?";

	$q = ibase_prepare($db, $sql);
	$r = ibase_execute($q, 1, 'test table not created with isql');
	var_dump(ibase_fetch_assoc($r));
	ibase_free_result($r);
	
	$r = ibase_execute($q, 1, 'test table not created with isql', 1);
	var_dump(ibase_fetch_assoc($r));
	ibase_free_result($r);
	
	$r = ibase_execute($q, 1);
	var_dump(ibase_fetch_assoc($r));

?>
--EXPECTF--
array(2) {
  ["I"]=>
  int(1)
  ["C"]=>
  string(32) "test table not created with isql"
}

Notice: ibase_execute(): Statement expects 2 arguments, 3 given in %s on line %d
array(2) {
  ["I"]=>
  int(1)
  ["C"]=>
  string(32) "test table not created with isql"
}

Warning: ibase_execute(): Statement expects 2 arguments, 1 given in %s on line %d

Warning: ibase_fetch_assoc() expects parameter 1 to be resource, boolean given in %s on line %d
NULL
