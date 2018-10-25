--TEST--
Test piecewise fetch of CLOBs equal to, and larger than PHP_OCI_LOB_BUFFER_SIZE
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';
require dirname(__FILE__).'/create_table.inc';

function insert_verify($c, $tn, $id, $length)
{
    // Insert the data
    $ora_sql = "INSERT INTO
                       ".$tn." (id, clob)
                      VALUES (".$id.", empty_clob())
                      RETURNING
                               clob
                      INTO :v_clob ";

    $statement = oci_parse($c,$ora_sql);
    $clob = oci_new_descriptor($c,OCI_D_LOB);
    oci_bind_by_name($statement,":v_clob", $clob, -1, OCI_B_CLOB);
    oci_execute($statement, OCI_DEFAULT);

    $data = str_pad("x", $length, "x");
    $clob->write($data);

    // Verify the data
    $select_sql = "SELECT clob FROM ".$tn." where id = ".$id;
    $s = oci_parse($c, $select_sql);
    oci_execute($s);

    $row = oci_fetch_array($s, OCI_RETURN_LOBS);

    var_dump(strlen($row[0]));
}

echo "Test 1: A CLOB with an even number of bytes\n";
insert_verify($c, $schema.$table_name, 1, 1050000);

echo "Test 2: A CLOB with an odd number of bytes\n";
insert_verify($c, $schema.$table_name, 2, 1050001);

echo "Test 3: A CLOB of 1048576 bytes (== size of PHP_OCI_LOB_BUFFER_SIZE at time of test creation)\n";
insert_verify($c, $schema.$table_name, 3, 1048576);

echo "Test 4: A CLOB of 1049028 bytes (the value used for chunks in the code)\n";
insert_verify($c, $schema.$table_name, 4, 1049028);

echo "Test 5: A CLOB of 1049028-1 bytes\n";
insert_verify($c, $schema.$table_name, 5, 1049028-1);

echo "Test 6: A CLOB of 1049028+1 bytes\n";
insert_verify($c, $schema.$table_name, 6, 1049028+1);

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECT--
Test 1: A CLOB with an even number of bytes
int(1050000)
Test 2: A CLOB with an odd number of bytes
int(1050001)
Test 3: A CLOB of 1048576 bytes (== size of PHP_OCI_LOB_BUFFER_SIZE at time of test creation)
int(1048576)
Test 4: A CLOB of 1049028 bytes (the value used for chunks in the code)
int(1049028)
Test 5: A CLOB of 1049028-1 bytes
int(1049027)
Test 6: A CLOB of 1049028+1 bytes
int(1049029)
Done
