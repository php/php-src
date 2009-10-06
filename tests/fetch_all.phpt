--TEST--
oci_fetch_all()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table fetch_all_tab",
    "create table fetch_all_tab (id number, value number)",
    "insert into fetch_all_tab (id, value) values (1,1)",
    "insert into fetch_all_tab (id, value) values (1,1)",
    "insert into fetch_all_tab (id, value) values (1,1)"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	$r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		if (!in_array($m['code'], array(   // ignore expected errors
                    942 // table or view does not exist
                ))) {
			echo $stmt . PHP_EOL . $m['message'] . PHP_EOL;
		}
	}
}

if (!($s = oci_parse($c, "select * from fetch_all_tab"))) {
	die("oci_parse(select) failed!\n");
}

/* oci_fetch_all */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}
var_dump(oci_fetch_all($s, $all));
var_dump($all);

/* ocifetchstatement */
if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

var_dump(ocifetchstatement($s, $all));
var_dump($all);

// Cleanup

$stmtarray = array(
    "drop table fetch_all_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}
	
echo "Done\n";
?>
--EXPECTF--
int(3)
array(2) {
  [%u|b%"ID"]=>
  array(3) {
    [0]=>
    %unicode|string%(1) "1"
    [1]=>
    %unicode|string%(1) "1"
    [2]=>
    %unicode|string%(1) "1"
  }
  [%u|b%"VALUE"]=>
  array(3) {
    [0]=>
    %unicode|string%(1) "1"
    [1]=>
    %unicode|string%(1) "1"
    [2]=>
    %unicode|string%(1) "1"
  }
}
int(3)
array(2) {
  [%u|b%"ID"]=>
  array(3) {
    [0]=>
    %unicode|string%(1) "1"
    [1]=>
    %unicode|string%(1) "1"
    [2]=>
    %unicode|string%(1) "1"
  }
  [%u|b%"VALUE"]=>
  array(3) {
    [0]=>
    %unicode|string%(1) "1"
    [1]=>
    %unicode|string%(1) "1"
    [2]=>
    %unicode|string%(1) "1"
  }
}
Done
