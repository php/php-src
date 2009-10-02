--TEST--
ocidefinebyname()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__)."/connect.inc");

// Initialize

$stmtarray = array(
    "drop table define_old_tab",
    "create table define_old_tab (string varchar(10))",
    "insert into define_old_tab (string) values ('some')",
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

$stmt = ociparse($c, "select string from define_old_tab");

/* the define MUST be done BEFORE ociexecute! */

$string = '';
ocidefinebyname($stmt, "STRING", $string, 20);

ociexecute($stmt);

while (ocifetch($stmt)) {
	var_dump($string);
}

// Cleanup

$stmtarray = array(
    "drop table define_old_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}


echo "Done\n";

?>
--EXPECTF--
%unicode|string%(4) "some"
Done
