--TEST--
oci_define_by_name() on partial number of columns
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table define4_tab",
    "create table define4_tab (value number, string varchar(10))",
    "insert into define4_tab (value, string) values (1234, 'some')",
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

$stmt = oci_parse($c, "select value, string from define4_tab");

echo "Test 1\n";
// Only one of the two columns is defined
var_dump(oci_define_by_name($stmt, "STRING", $string));

oci_execute($stmt);

echo "Test 2\n";

while (oci_fetch($stmt)) {
	var_dump(oci_result($stmt, 'VALUE'));
	var_dump($string);
	var_dump(oci_result($stmt, 'STRING'));
	var_dump($string);
	var_dump(oci_result($stmt, 'VALUE'));
	var_dump(oci_result($stmt, 'STRING'));
}

echo "Test 3\n";
var_dump(oci_free_statement($stmt));
var_dump($string);
var_dump(oci_result($stmt, 'STRING'));

// Cleanup

$stmtarray = array(
    "drop table define4_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

echo "Done\n";

?>
--EXPECTF--
Test 1
bool(true)
Test 2
%unicode|string%(4) "1234"
%unicode|string%(4) "some"
%unicode|string%(4) "some"
%unicode|string%(4) "some"
%unicode|string%(4) "1234"
%unicode|string%(4) "some"
Test 3
bool(true)
%unicode|string%(4) "some"

Warning: oci_result(): %d is not a valid oci8 statement resource in %s on line %d
bool(false)
Done

