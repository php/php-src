--TEST--
Test LOB->read(), LOB->seek() and LOB->tell() with nul bytes in data
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(dirname(__FILE__).'/skipif.inc');
?>
--FILE--
<?php

require dirname(__FILE__).'/connect.inc';
require dirname(__FILE__).'/create_table.inc';

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (blob)
                      VALUES (empty_blob())
                      RETURNING
                               blob
                      INTO :v_blob ";

$statement = oci_parse($c,$ora_sql);
$blob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_blob", $blob,-1,OCI_B_BLOB);
oci_execute($statement, OCI_DEFAULT);

$blob->write("test");
$blob->tell();
$blob->seek(10, OCI_SEEK_CUR);
$blob->write("string");
$blob->flush();

$select_sql = "SELECT blob FROM ".$schema.$table_name;
$s = oci_parse($c, $select_sql);
oci_execute($s);
$row = oci_fetch_array($s);

$row[0]->read(3);
echo " 1. ".$row[0]->tell(). "\n";

$row[0]->read(3);
echo " 2. ".$row[0]->tell(). "\n";

$row[0]->read(3);
echo " 3. ".$row[0]->tell(). "\n";

$row[0]->read(6);
echo " 4. ".$row[0]->tell(). "\n";

$row[0]->read(4);
echo " 5. ".$row[0]->tell(). "\n";

// Read past end
$row[0]->read(5);
echo " 6. ".$row[0]->tell(). "\n";

$row[0]->read(1);
echo " 8. ".$row[0]->tell(). "\n";

// Now seek
$row[0]->seek(1);
echo " 9. ".$row[0]->tell(). "\n";

$row[0]->seek(8);
echo "10. ".$row[0]->tell(). "\n";

$row[0]->seek(20);
echo "11. ".$row[0]->tell(). "\n";

// Seek past end
$row[0]->seek(25);
echo "12. ".$row[0]->tell(). "\n";

// Seek past end
$row[0]->seek(2, OCI_SEEK_SET);
echo "13. ".$row[0]->tell(). "\n";

// Move on 2 more
$row[0]->seek(2, OCI_SEEK_CUR);
echo "14. ".$row[0]->tell(). "\n";

// Move 3 past the end
$row[0]->seek(3, OCI_SEEK_END);
echo "15. ".$row[0]->tell(). "\n";

// Move 4 before the end
$row[0]->seek(-4, OCI_SEEK_END);
echo "16. ".$row[0]->tell(). "\n";

require dirname(__FILE__).'/drop_table.inc';

echo "Done\n";

?>
--EXPECT--
 1. 3
 2. 6
 3. 9
 4. 15
 5. 19
 6. 20
 8. 20
 9. 1
10. 8
11. 20
12. 25
13. 2
14. 4
15. 23
16. 16
Done
