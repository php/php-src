--TEST--
Bug #43492 (Nested cursor leaks after related bug #44206 fixed)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

// This test is similar to bug43492.phpt without the explicit free.
// Now that bug 44206 is fixed an automatic clean up will occur -
// though it is still recommended in practice.

require __DIR__.'/connect.inc';

$stmtarray = array(
    "DROP table bug43492_tab",
    "CREATE TABLE bug43492_tab(col1 VARCHAR2(1))",
    "INSERT INTO bug43492_tab VALUES ('A')",
    "INSERT INTO bug43492_tab VALUES ('B')",
    "INSERT INTO bug43492_tab VALUES ('C')",
    "INSERT INTO bug43492_tab VALUES ('D')",
    "INSERT INTO bug43492_tab VALUES ('E')",
    "INSERT INTO bug43492_tab VALUES ('F')",
    "INSERT INTO bug43492_tab VALUES ('G')",
    "INSERT INTO bug43492_tab VALUES ('H')",
    "INSERT INTO bug43492_tab VALUES ('I')",
    "INSERT INTO bug43492_tab VALUES ('J')"
);

oci8_test_sql_execute($c, $stmtarray);

function fetch($c, $i) {
    $s = ociparse($c, 'select cursor(select * from bug43492_tab) c from bug43492_tab');
    ociexecute($s, OCI_DEFAULT);
    ocifetchinto($s, $result, OCI_ASSOC);
    ociexecute($result['C'], OCI_DEFAULT);
    return $result['C'];
}

for($i = 0; $i < 300; $i++) {
    $cur = fetch($c, $i);
    for($j = 0; $j < 10; $j++) {
        ocifetchinto($cur, $row, OCI_NUM);
        echo "$row[0] ";
    }
    echo "\n";
    ocifreestatement($cur);
}

echo "Done\n";

// Cleanup

$stmtarray = array(
    "DROP table bug43492_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECT--
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
A B C D E F G H I J 
Done
