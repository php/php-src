--TEST--
Bug #49560 (LOB resource destructor and refcount test)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); 
require(dirname(__FILE__).'/details.inc');
if ($stress_test !== true) die ('skip Slow test not run when $stress_test is FALSE');
?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table lob_043_tab",
	"create table lob_043_tab(id number, c1 clob)",
	"begin 
       for i in 1..50000 loop
         insert into lob_043_tab (id, c1) values (i, i || ' abcdefghijklmnopq');
      end loop;
     end;",
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

function f1($c)
{
    $s = oci_parse($c, 'select id, c1 from lob_043_tab order by id');
    oci_execute($s);
    $r = array();
    while (($row = oci_fetch_array($s, OCI_RETURN_NULLS+OCI_ASSOC+OCI_RETURN_LOBS)) !== false) {
        $r[] = $row['C1'];
    }
    echo "f1 ended\n";
    return $r;
}

function f2($c)
{
    $s = oci_parse($c, 'select id, c1 from lob_043_tab order by id');
    oci_execute($s);
    $r = array();
    while (($row = oci_fetch_array($s, OCI_RETURN_NULLS+OCI_ASSOC)) !== false) {
        $r[] = $row['C1'];
    }
    echo "f2 ended\n";
    return $r;
}

echo "Test 1\n";
$r = f1($c);
/*
  foreach ($r as $v) {
  echo $v, "\n";
  }
*/

echo "Test 2\n";
$r = f2($c);
/*
  foreach ($r as $v) {
  echo $v->load(), "\n";
  }
*/

// Clean up

$stmtarray = array(
	"drop table lob_043_tab"
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
Test 1
f1 ended
Test 2
f2 ended
===DONE===
