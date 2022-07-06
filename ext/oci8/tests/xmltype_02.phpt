--TEST--
Basic XMLType test #2
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
if (!extension_loaded("simplexml")) die ("skip no simplexml extension");
?>
--FILE--
<?php

require(__DIR__.'/connect.inc');

// Initialization

$stmtarray = array(
    "drop table xmltype_02_tab",
    "create table xmltype_02_tab (warehouse_id number, warehouse_spec xmltype)",
);

oci8_test_sql_execute($c, $stmtarray);

// Run Test


$id = 1;

// Delete any current entry
$s = oci_parse($c, "delete from xmltype_02_tab where warehouse_id = :id");
oci_bind_by_name($s, ':id', $id);
oci_execute($s);

// XML data to be inserted
$xml =<<<EOF
<?xml version="1.0"?>
<Warehouse>
<WarehouseId>1</WarehouseId>
<WarehouseName>Southlake, Texas</WarehouseName>
<Building>Owned</Building>
<Area>25000</Area>
<Docks>2</Docks>
<DockType>Rear load</DockType>
<WaterAccess>true</WaterAccess>
<RailAccess>N</RailAccess>
<Parking>Street</Parking>
<VClearance>10</VClearance>
</Warehouse>
EOF;

echo "Test 1 Insert new XML data using a temporary CLOB\n";
$s = oci_parse($c,
    "insert into xmltype_02_tab (warehouse_id, warehouse_spec)
     values (:id, XMLType(:clob))");
oci_bind_by_name($s, ':id', $id);
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':clob', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary($xml);
oci_execute($s);
$lob->close();

// Query the row back
$s = oci_parse($c, 'select xmltype.getclobval(warehouse_spec)
                    from xmltype_02_tab where warehouse_id = :id');
$r = oci_bind_by_name($s, ':id', $id);
oci_execute($s);
$row = oci_fetch_array($s, OCI_NUM);

var_dump($row);

echo "Test 2 Manipulate the data using SimpleXML\n";

$sx = simplexml_load_string($row[0]->load());
$row[0]->free();
var_dump($sx);

$sx->Docks -= 1;  // change the data

var_dump($sx);

echo "Test 3: Update changes using a temporary CLOB\n";

$s = oci_parse($c, 'update xmltype_02_tab
                    set warehouse_spec = XMLType(:clob)
                    where warehouse_id = :id');
oci_bind_by_name($s, ':id', $id);
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':clob', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary($sx->asXml());
oci_execute($s);
$lob->close();

// Query the changed row back and print it
$s = oci_parse($c, 'select xmltype.getclobval(warehouse_spec)
                    from xmltype_02_tab where warehouse_id = :id');
$r = oci_bind_by_name($s, ':id', $id);
oci_execute($s);
$row = oci_fetch_array($s, OCI_NUM);
var_dump($row[0]->load());
$row[0]->free();

// Clean up

$stmtarray = array(
    "drop table xmltype_02_tab"
);

oci8_test_sql_execute($c, $stmtarray);

?>
--EXPECTF--
Test 1 Insert new XML data using a temporary CLOB
array(1) {
  [0]=>
  object(OCILob)#%d (1) {
    ["descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
Test 2 Manipulate the data using SimpleXML
object(SimpleXMLElement)#%d (10) {
  ["WarehouseId"]=>
  string(1) "1"
  ["WarehouseName"]=>
  string(16) "Southlake, Texas"
  ["Building"]=>
  string(5) "Owned"
  ["Area"]=>
  string(5) "25000"
  ["Docks"]=>
  string(1) "2"
  ["DockType"]=>
  string(9) "Rear load"
  ["WaterAccess"]=>
  string(4) "true"
  ["RailAccess"]=>
  string(1) "N"
  ["Parking"]=>
  string(6) "Street"
  ["VClearance"]=>
  string(2) "10"
}
object(SimpleXMLElement)#%d (10) {
  ["WarehouseId"]=>
  string(1) "1"
  ["WarehouseName"]=>
  string(16) "Southlake, Texas"
  ["Building"]=>
  string(5) "Owned"
  ["Area"]=>
  string(5) "25000"
  ["Docks"]=>
  string(1) "1"
  ["DockType"]=>
  string(9) "Rear load"
  ["WaterAccess"]=>
  string(4) "true"
  ["RailAccess"]=>
  string(1) "N"
  ["Parking"]=>
  string(6) "Street"
  ["VClearance"]=>
  string(2) "10"
}
Test 3: Update changes using a temporary CLOB
string(%d) "<?xml version="1.0"?>
<Warehouse>
%sWarehouseId>1</WarehouseId>
%sWarehouseName>Southlake, Texas</WarehouseName>
%sBuilding>Owned</Building>
%sArea>25000</Area>
%sDocks>1</Docks>
%sDockType>Rear load</DockType>
%sWaterAccess>true</WaterAccess>
%sRailAccess>N</RailAccess>
%sParking>Street</Parking>
%sVClearance>10</VClearance>
</Warehouse>
"
