--TEST--
Bug #43492 (Nested cursor leaks)
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';

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

foreach ($stmtarray as $stmt) {
    $s = oci_parse($c, $stmt);
    @oci_execute($s);
}

/*

While fetching data from a ref cursor, the parent statement needs to
be around.  Also when the parent statement goes out of scope, it is
not automatically released which causes a cursor leak.

If either or both of the lines marked (*) are removed, then the script
will fail with the error "ORA-01000: maximum open cursors exceeded".

*/

function fetch($c, $i) {
    global $s;   //  (*) Allow parent statement to be available when child is used
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
    ocifreestatement($s);   // (*) Free the parent statement cleanly
}

echo "Done\n";

// Cleanup

$stmtarray = array(
    "DROP table bug43492_tab"
);

foreach ($stmtarray as $stmt) {
    $s = oci_parse($c, $stmt);
    @oci_execute($s);
}

oci_close($c);

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
