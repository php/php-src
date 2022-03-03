--TEST--
oci_lob_write()/erase()/read() with CLOBs
--EXTENSIONS--
oci8
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';
require __DIR__.'/create_table.inc';

$ora_sql = "INSERT INTO
                       ".$schema.$table_name." (Clob)
                      VALUES (empty_Clob())
                      RETURNING
                               clob
                      INTO :v_clob ";

$statement = oci_parse($c,$ora_sql);
$clob = oci_new_descriptor($c,OCI_D_LOB);
oci_bind_by_name($statement,":v_clob", $clob,-1,OCI_B_CLOB);
oci_execute($statement, OCI_DEFAULT);

var_dump($clob);

$str = "%0%0%0%0%0this is a biiiig faaat test string. why are you reading it, I wonder? =)";
var_dump($clob->write($str));
var_dump($clob->erase(10,20));

oci_commit($c);

$select_sql = "SELECT clob FROM ".$schema.$table_name." FOR UPDATE";
$s = oci_parse($c, $select_sql);
oci_execute($s, OCI_DEFAULT);

var_dump($row = oci_fetch_array($s));

var_dump($row[0]->read(2));
var_dump($row[0]->read(5));
var_dump($row[0]->read(50));

var_dump($clob->erase());

echo "\nInvalid values\n";

echo "\nTest 1\n";
try {
    var_dump($clob->erase(-10));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "\nTest 2\n";
try {
    var_dump($clob->erase(10,-20));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "\nTest 3\n";
try {
    var_dump($clob->erase(-10,-20));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "\nTest 4\n";
try {
    // ORA-22990: LOB locators cannot span transactions
    var_dump(oci_lob_erase($clob));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
    
echo "\nTest 5\n";
try {
    var_dump(oci_lob_erase($clob,-10));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "\nTest 6\n";
try {
    var_dump(oci_lob_erase($clob,10,-20));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "\nTest 7\n";
try {
    var_dump(oci_lob_erase($clob,-10,-20));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

echo "\nTest 8\n";
unset($clob->descriptor);
var_dump(oci_lob_erase($clob,10,20));

require __DIR__.'/drop_table.inc';

echo "Done\n";

?>
--EXPECTF--
object(OCILob)#%d (1) {
  ["descriptor"]=>
  resource(%d) of type (oci8 descriptor)
}
int(82)
int(20)
array(2) {
  [0]=>
  object(OCILob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
  ["CLOB"]=>
  object(OCILob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
string(2) "%s"
string(5) "%s"
string(50) "%s                    at test string. why are you"

Warning: OCILob::erase(): ORA-22990: %s in %s on line %d
bool(false)

Invalid values

Test 1
OCILob::erase(): Argument #1 ($offset) must be greater than or equal to 0

Test 2
OCILob::erase(): Argument #2 ($length) must be greater than or equal to 0

Test 3
OCILob::erase(): Argument #1 ($offset) must be greater than or equal to 0

Test 4

Warning: oci_lob_erase(): ORA-22990: %s in %s on line %d
bool(false)

Test 5
oci_lob_erase(): Argument #2 ($offset) must be greater than or equal to 0

Test 6
oci_lob_erase(): Argument #3 ($length) must be greater than or equal to 0

Test 7
oci_lob_erase(): Argument #2 ($offset) must be greater than or equal to 0

Test 8

Warning: oci_lob_erase(): Unable to find descriptor property in %s on line %d
bool(false)
Done
