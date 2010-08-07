--TEST--
Basic XMLType test #2
--SKIPIF--
<?php if (!extension_loaded('oci8')) die ("skip no oci8 extension"); ?>
--FILE--
<?php

require(dirname(__FILE__).'/connect.inc');

// Initialization

$stmtarray = array(
	"drop table xmltype_02_tab",
	"create table xmltype_02_tab (warehouse_id number, warehouse_spec xmltype)",
);

foreach ($stmtarray as $stmt) {
	$s = oci_parse($c, $stmt);
	$r = @oci_execute($s);
	if (!$r) {
		$m = oci_error($s);
		if (!in_array($m['code'], array(   // ignore expected errors
			   942 // table or view does not exist
			, 2289 // sequence does not exist
			, 4080 // trigger does not exist
                ))) {
			echo $stmt . PHP_EOL . $m['message'] . PHP_EOL;
		}
	}
}

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

$sx = simplexml_load_string((binary)$row[0]->load());
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

//require(dirname(__FILE__).'/drop_table.inc');

$stmtarray = array(
	"drop table xmltype_02_tab"
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
Test 1 Insert new XML data using a temporary CLOB
array(1) {
  [0]=>
  object(OCI-Lob)#%d (1) {
    [%u|b%"descriptor"]=>
    resource(%d) of type (oci8 descriptor)
  }
}
Test 2 Manipulate the data using SimpleXML
object(SimpleXMLElement)#%d (10) {
  [%u|b%"WarehouseId"]=>
  %unicode|string%(1) "1"
  [%u|b%"WarehouseName"]=>
  %unicode|string%(16) "Southlake, Texas"
  [%u|b%"Building"]=>
  %unicode|string%(5) "Owned"
  [%u|b%"Area"]=>
  %unicode|string%(5) "25000"
  [%u|b%"Docks"]=>
  %unicode|string%(1) "2"
  [%u|b%"DockType"]=>
  %unicode|string%(9) "Rear load"
  [%u|b%"WaterAccess"]=>
  %unicode|string%(4) "true"
  [%u|b%"RailAccess"]=>
  %unicode|string%(1) "N"
  [%u|b%"Parking"]=>
  %unicode|string%(6) "Street"
  [%u|b%"VClearance"]=>
  %unicode|string%(2) "10"
}
object(SimpleXMLElement)#%d (10) {
  [%u|b%"WarehouseId"]=>
  %unicode|string%(1) "1"
  [%u|b%"WarehouseName"]=>
  %unicode|string%(16) "Southlake, Texas"
  [%u|b%"Building"]=>
  %unicode|string%(5) "Owned"
  [%u|b%"Area"]=>
  %unicode|string%(5) "25000"
  [%u|b%"Docks"]=>
  %unicode|string%(1) "1"
  [%u|b%"DockType"]=>
  %unicode|string%(9) "Rear load"
  [%u|b%"WaterAccess"]=>
  %unicode|string%(4) "true"
  [%u|b%"RailAccess"]=>
  %unicode|string%(1) "N"
  [%u|b%"Parking"]=>
  %unicode|string%(6) "Street"
  [%u|b%"VClearance"]=>
  %unicode|string%(2) "10"
}
Test 3: Update changes using a temporary CLOB
%unicode|string%(331) "<?xml version="1.0"?>
<Warehouse>
<WarehouseId>1</WarehouseId>
<WarehouseName>Southlake, Texas</WarehouseName>
<Building>Owned</Building>
<Area>25000</Area>
<Docks>1</Docks>
<DockType>Rear load</DockType>
<WaterAccess>true</WaterAccess>
<RailAccess>N</RailAccess>
<Parking>Street</Parking>
<VClearance>10</VClearance>
</Warehouse>
"
===DONE===