--TEST--
Bug #71600 (oci_fetch_all result in segfault when select more than 8 columns)
--EXTENSIONS--
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => true);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

// Initialize

$stmtarray = array(
         "create table bug71600_tab (col1 number, col2 number, col3 number,
                                     col4 number, col5 number, col6 number,
                                     col7 number, col8 number, col9 number)",
         "insert into bug71600_tab values(1, 2, 3, 4, 5, 6, 7, 8, 9)",
         "insert into bug71600_tab values(11, 12, 13, 14, 15, 16, 17, 18, 19)"
);

oci8_test_sql_execute($c, $stmtarray);

// Run test

$sql = "select col1,col2,col3,col4,col5,col6,col7,col8,col9 from bug71600_tab";

echo "Test 1\n";
$stmt = oci_parse($c, $sql);

echo "Executing SELECT statament...\n";
oci_execute($stmt,OCI_DEFAULT);

echo "Fetching data by columns...\n";
oci_fetch_all($stmt, $result);
oci_free_statement($stmt);

$rsRows=(count($result,1)/($rows = count($result,0)))-1;
echo "$rsRows Records Found\n";
$rsCount=0;
while($rsCount < $rsRows)
{
  $col1   =$result['COL1'][$rsCount];
  $col9   =$result['COL9'][$rsCount];
  echo "$rsCount|$col1|$col9\n";
  $rsCount++;
}

echo "Test 2\n";
$stmt = oci_parse($c, $sql);

echo "Re-executing SELECT statament...\n";
oci_execute($stmt,OCI_DEFAULT);

echo "Fetching data by rows...\n";
oci_fetch_all($stmt, $result, 0, -1, OCI_FETCHSTATEMENT_BY_ROW);
oci_free_statement($stmt);

$rsRows=count($result,0);
echo "$rsRows Records Found\n";
$rsCount=0;
while($rsCount < $rsRows)
{
  $col1 = $result[$rsCount]['COL1'];
  $col9 = $result[$rsCount]['COL9'];
  echo "$rsCount|$col1|$col9\n";
  $rsCount++;
}


// Cleanup

$stmtarray = array(
    "drop table bug71600_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECT--
Test 1
Executing SELECT statament...
Fetching data by columns...
2 Records Found
0|1|9
1|11|19
Test 2
Re-executing SELECT statament...
Fetching data by rows...
2 Records Found
0|1|9
1|11|19
