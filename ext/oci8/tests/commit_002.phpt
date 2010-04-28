--TEST--
Test oci_commit failure
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table commit_002_tab",
	"create table commit_002_tab
     ( x int constraint commit_002_tab_check_x check ( x > 0 ) deferrable initially immediate,
       y int constraint commit_002_tab_check_y check ( y > 0 ) deferrable initially deferred)"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	$r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		if (!in_array($m['code'], array(   // ignore expected errors
			    942 // table or view does not exist
			,  2289 // sequence does not exist
			,  4080 // trigger does not exist
			, 38802 // edition does not exist
                ))) {
			echo $stmt . PHP_EOL . $m['message'] . PHP_EOL;
		}
	}
}

// Run Test

echo "First Insert\n";
$s = oci_parse($c, "insert into commit_002_tab values (-1, 1)");
$r = @oci_execute($s, OCI_DEFAULT);
if (!$r) {
    $m = oci_error($s);
    echo 'Could not execute: '. $m['message'] . "\n";
}
$r = oci_commit($c);
if (!$r) {
    $m = oci_error($c);
    echo 'Could not commit: '. $m['message'] . "\n";
}


echo "Second Insert\n";
$s = oci_parse($c, "insert into commit_002_tab values (1, -1)");
$r = @oci_execute($s, OCI_NO_AUTO_COMMIT);
if (!$r) {
    $m = oci_error($s);
    echo 'Could not execute: '. $m['message'] . "\n";
}
$r = oci_commit($c);
if (!$r) {
    $m = oci_error($c);
    echo 'Could not commit: '. $m['message'] . "\n";
}


// Clean up

$stmtarray = array(
	"drop table commit_002_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

oci_close($c);

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
First Insert
Could not execute: ORA-02290: %s (%s.COMMIT_002_TAB_CHECK_X) %s
Second Insert

Warning: oci_commit(): ORA-02091: %s
ORA-02290: %s (%s.COMMIT_002_TAB_CHECK_Y) %s in %scommit_002.php on line %d
Could not commit: ORA-02091: %s
ORA-02290: %s (%s.COMMIT_002_TAB_CHECK_Y) %s
===DONE===
