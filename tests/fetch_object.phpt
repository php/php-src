--TEST--
oci_fetch_object()
--SKIPIF--
<?php if (!extension_loaded('oci8')) die("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table fetch_object_tab",
	"create table fetch_object_tab (\"caseSensitive\" number, secondcol varchar2(20), anothercol char(15))",
	"insert into fetch_object_tab values (123, '1st row col2 string', '1 more text')",
	"insert into fetch_object_tab values (456, '2nd row col2 string', '2 more text')",
	"insert into fetch_object_tab values (789, '3rd row col2 string', '3 more text')",
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

echo "Test 1\n";

if (!($s = oci_parse($c, 'select * from fetch_object_tab'))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

while ($row = oci_fetch_object($s)) {
	var_dump($row);
}

echo "Test 2\n";

if (!($s = oci_parse($c, 'select * from fetch_object_tab'))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

while ($row = oci_fetch_object($s)) {
    echo $row->caseSensitive . "\n";
    echo $row->SECONDCOL . "\n";
    echo $row->ANOTHERCOL . "\n";
}

echo "Test 3\n";

if (!($s = oci_parse($c, 'select * from fetch_object_tab where rownum < 2 order by "caseSensitive"'))) {
	die("oci_parse(select) failed!\n");
}

if (!oci_execute($s)) {
	die("oci_execute(select) failed!\n");
}

$row = oci_fetch_object($s);
echo $row->caseSensitive . "\n";
echo $row->CASESENSITIVE . "\n";

// Clean up

$stmtarray = array(
	"drop table fetch_object_tab"
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	oci_execute($s);
}

?>
===DONE===
<?php exit(0); ?>
--EXPECTF--
Test 1
object(stdClass)#1 (3) {
  [%u|b%"caseSensitive"]=>
  %unicode|string%(3) "123"
  [%u|b%"SECONDCOL"]=>
  %unicode|string%(19) "1st row col2 string"
  [%u|b%"ANOTHERCOL"]=>
  %unicode|string%(15) "1 more text    "
}
object(stdClass)#2 (3) {
  [%u|b%"caseSensitive"]=>
  %unicode|string%(3) "456"
  [%u|b%"SECONDCOL"]=>
  %unicode|string%(19) "2nd row col2 string"
  [%u|b%"ANOTHERCOL"]=>
  %unicode|string%(15) "2 more text    "
}
object(stdClass)#1 (3) {
  [%u|b%"caseSensitive"]=>
  %unicode|string%(3) "789"
  [%u|b%"SECONDCOL"]=>
  %unicode|string%(19) "3rd row col2 string"
  [%u|b%"ANOTHERCOL"]=>
  %unicode|string%(15) "3 more text    "
}
Test 2
123
1st row col2 string
1 more text    
456
2nd row col2 string
2 more text    
789
3rd row col2 string
3 more text    
Test 3
123

Notice: Undefined property: stdClass::$CASESENSITIVE in %sfetch_object.php on line %d

===DONE===
