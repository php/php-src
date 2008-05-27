--TEST--
Bug #41069 (Oracle crash with certain data over a DB-link when prefetch memory limit used - Oracle bug 6039623)
--SKIPIF--
<?php 
if (!extension_loaded('oci8')) die ("skip no oci8 extension"); 
require(dirname(__FILE__).'/details.inc');
if (empty($dbase)) die ("skip requires network connection alias for DB link loopback");
if ($test_drcp) die("skip DRCP does not support shared database links");
?>
--INI--
oci8.default_prefetch=5
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"alter session set nls_date_format = 'MM/DD/YYYY'",

	"drop database link bug41069_dblink",

	"drop table bug41069_tab",

	"create shared database link bug41069_dblink authenticated by $user identified by $password using '$dbase'",

	"create table bug41069_tab
	(
		c1  number(20),
		c2  varchar2(60 byte),
		c3  varchar2(1000 byte),
		c4  varchar2(255 byte),
		c5  varchar2(2 byte),
		c6  varchar2(1 byte),
		c7  varchar2(255 byte),
		c8  varchar2(50 byte),
		c9  date,
		c10 date,
		c12 number(20),
		c13 varchar2(20 byte),
		c15 varchar2(50 byte)
	 )",

	"insert into bug41069_tab (c1, c2, c5, c6, c9, c10, c12, c15)	values
	(111, 'aaaaaaa', 'b', 'c', '01/17/2008', '01/07/2017', 2222, 'zzzzzzzzzz')",

	"insert into bug41069_tab (c1, c2, c3, c4, c5, c6, c7, c9, c10, c12, c13, c15) values
	(112, 'aaaaaaa', 'bbbbbbbb', 'ccccccc', 'd', 'e', 'rrrrrrr', '04/16/2007', '04/16/2007', 2223, 'xxxxxxxx', 'zzzzzzzz')",

	"insert into bug41069_tab (c1, c2, c3, c4, c5, c6, c7, c9, c10, c12, c15)	values
	(113, 'aaaaaaa', 'bbbbbbbbbb', 'cccccc', 'e', 'f', 'dddd', '12/04/2006', '12/04/2006', 2224, 'zzzzzzz')"
				   );
						 
foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	@oci_execute($s);
}


// Run Tests

echo "Test 1: non-DB link case that always worked\n";
$stid = oci_parse($c, 'select * from bug41069_tab order by c1');
oci_execute($stid, OCI_DEFAULT);
oci_fetch_all($stid, $results, 0, -1, OCI_ASSOC+OCI_FETCHSTATEMENT_BY_ROW);
var_dump($results);

echo "Test 2: Should not crash\n";
$stid = oci_parse($c, 'select * from bug41069_tab@bug41069_dblink order by c1');
oci_execute($stid, OCI_DEFAULT);
oci_fetch_all($stid, $results, 0, -1, OCI_ASSOC+OCI_FETCHSTATEMENT_BY_ROW);
var_dump($results);

// Cleanup

$c = oci_new_connect($user, $password, $dbase);

$stmtarray = array(
	"drop database link bug41069_dblink",
	"drop table bug41069_tab"
				   );

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

oci_close($c);

echo "Done\n";

?>
--EXPECT--
Test 1: non-DB link case that always worked
array(3) {
  [0]=>
  array(13) {
    [u"C1"]=>
    unicode(3) "111"
    [u"C2"]=>
    unicode(7) "aaaaaaa"
    [u"C3"]=>
    NULL
    [u"C4"]=>
    NULL
    [u"C5"]=>
    unicode(1) "b"
    [u"C6"]=>
    unicode(1) "c"
    [u"C7"]=>
    NULL
    [u"C8"]=>
    NULL
    [u"C9"]=>
    unicode(10) "01/17/2008"
    [u"C10"]=>
    unicode(10) "01/07/2017"
    [u"C12"]=>
    unicode(4) "2222"
    [u"C13"]=>
    NULL
    [u"C15"]=>
    unicode(10) "zzzzzzzzzz"
  }
  [1]=>
  array(13) {
    [u"C1"]=>
    unicode(3) "112"
    [u"C2"]=>
    unicode(7) "aaaaaaa"
    [u"C3"]=>
    unicode(8) "bbbbbbbb"
    [u"C4"]=>
    unicode(7) "ccccccc"
    [u"C5"]=>
    unicode(1) "d"
    [u"C6"]=>
    unicode(1) "e"
    [u"C7"]=>
    unicode(7) "rrrrrrr"
    [u"C8"]=>
    NULL
    [u"C9"]=>
    unicode(10) "04/16/2007"
    [u"C10"]=>
    unicode(10) "04/16/2007"
    [u"C12"]=>
    unicode(4) "2223"
    [u"C13"]=>
    unicode(8) "xxxxxxxx"
    [u"C15"]=>
    unicode(8) "zzzzzzzz"
  }
  [2]=>
  array(13) {
    [u"C1"]=>
    unicode(3) "113"
    [u"C2"]=>
    unicode(7) "aaaaaaa"
    [u"C3"]=>
    unicode(10) "bbbbbbbbbb"
    [u"C4"]=>
    unicode(6) "cccccc"
    [u"C5"]=>
    unicode(1) "e"
    [u"C6"]=>
    unicode(1) "f"
    [u"C7"]=>
    unicode(4) "dddd"
    [u"C8"]=>
    NULL
    [u"C9"]=>
    unicode(10) "12/04/2006"
    [u"C10"]=>
    unicode(10) "12/04/2006"
    [u"C12"]=>
    unicode(4) "2224"
    [u"C13"]=>
    NULL
    [u"C15"]=>
    unicode(7) "zzzzzzz"
  }
}
Test 2: Should not crash
array(3) {
  [0]=>
  array(13) {
    [u"C1"]=>
    unicode(3) "111"
    [u"C2"]=>
    unicode(7) "aaaaaaa"
    [u"C3"]=>
    NULL
    [u"C4"]=>
    NULL
    [u"C5"]=>
    unicode(1) "b"
    [u"C6"]=>
    unicode(1) "c"
    [u"C7"]=>
    NULL
    [u"C8"]=>
    NULL
    [u"C9"]=>
    unicode(10) "01/17/2008"
    [u"C10"]=>
    unicode(10) "01/07/2017"
    [u"C12"]=>
    unicode(4) "2222"
    [u"C13"]=>
    NULL
    [u"C15"]=>
    unicode(10) "zzzzzzzzzz"
  }
  [1]=>
  array(13) {
    [u"C1"]=>
    unicode(3) "112"
    [u"C2"]=>
    unicode(7) "aaaaaaa"
    [u"C3"]=>
    unicode(8) "bbbbbbbb"
    [u"C4"]=>
    unicode(7) "ccccccc"
    [u"C5"]=>
    unicode(1) "d"
    [u"C6"]=>
    unicode(1) "e"
    [u"C7"]=>
    unicode(7) "rrrrrrr"
    [u"C8"]=>
    NULL
    [u"C9"]=>
    unicode(10) "04/16/2007"
    [u"C10"]=>
    unicode(10) "04/16/2007"
    [u"C12"]=>
    unicode(4) "2223"
    [u"C13"]=>
    unicode(8) "xxxxxxxx"
    [u"C15"]=>
    unicode(8) "zzzzzzzz"
  }
  [2]=>
  array(13) {
    [u"C1"]=>
    unicode(3) "113"
    [u"C2"]=>
    unicode(7) "aaaaaaa"
    [u"C3"]=>
    unicode(10) "bbbbbbbbbb"
    [u"C4"]=>
    unicode(6) "cccccc"
    [u"C5"]=>
    unicode(1) "e"
    [u"C6"]=>
    unicode(1) "f"
    [u"C7"]=>
    unicode(4) "dddd"
    [u"C8"]=>
    NULL
    [u"C9"]=>
    unicode(10) "12/04/2006"
    [u"C10"]=>
    unicode(10) "12/04/2006"
    [u"C12"]=>
    unicode(4) "2224"
    [u"C13"]=>
    NULL
    [u"C15"]=>
    unicode(7) "zzzzzzz"
  }
}
Done
