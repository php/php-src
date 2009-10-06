--TEST--
oci_define_by_name() for statement re-execution
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table define5_tab",
    "create table define5_tab (id number, string varchar(10))",
    "insert into define5_tab (id, string) values (1, 'some')",
    "insert into define5_tab (id, string) values (2, 'thing')",
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

// Run test

echo "Test 1 - must do define before execute\n";
$stmt = oci_parse($c, "select string from define5_tab where id = 1");
oci_execute($stmt);
var_dump(oci_define_by_name($stmt, "STRING", $string));
while (oci_fetch($stmt)) {
	var_dump($string);  // gives NULL
	var_dump(oci_result($stmt, 'STRING'));
}

echo "Test 2 - normal define order\n";
$stmt = oci_parse($c, "select string from define5_tab where id = 1");
var_dump(oci_define_by_name($stmt, "STRING", $string));
oci_execute($stmt);

while (oci_fetch($stmt)) {
	var_dump($string);
}

echo "Test 3 - no new define done\n";
$stmt = oci_parse($c, "select string from define5_tab where id = 2");
oci_execute($stmt);
while (oci_fetch($stmt)) {
	var_dump($string); // not updated with new value
	var_dump(oci_result($stmt, 'STRING'));
}

// Cleanup

$stmtarray = array(
    "drop table define5_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

echo "Done\n";

?>
--EXPECTF--
Test 1 - must do define before execute
bool(true)
NULL
%unicode|string%(4) "some"
Test 2 - normal define order
bool(true)
%unicode|string%(4) "some"
Test 3 - no new define done
%unicode|string%(4) "some"
%unicode|string%(5) "thing"
Done

