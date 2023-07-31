--TEST--
Basic XMLType test
--EXTENSIONS--
simplexml
oci8
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require __DIR__.'/skipif.inc';
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

// Initialization

$stmtarray = array(
    "drop table xtt",
    "create table xtt
           (xt_id number, xt_spec xmltype)
           xmltype xt_spec store as clob",
    "insert into xtt (xt_id, xt_spec) values
      (1,
       xmltype('<?xml version=\"1.0\"?>
        <Xt>
          <XtId>1</XtId>
          <Size>Big</Size>
          <Area>12345</Area>
          <Hardness>20</Hardness>
          <Lip>Curved</Lip>
          <Color>Red</Color>
          <Nice>N</Nice>
          <Compact>Tiny</Compact>
          <Material>Steel</Material>
        </Xt>'))"
);

oci8_test_sql_execute($c, str_replace("\r", "", $stmtarray));

function do_query($c)
{
    $s = oci_parse($c, 'select XMLType.getClobVal(xt_spec)
                        from xtt where xt_id = 1');
    oci_execute($s);
    $row = oci_fetch_row($s);
    $data = $row[0]->load();
    var_dump($data);
    return($data);
}

// Check
echo "Initial Data\n";
$data = do_query($c);

// Manipulate the data using SimpleXML
$sx = simplexml_load_string($data);
$sx->Hardness = $sx->Hardness - 1;
$sx->Nice = 'Y';

// Insert changes using a temporary CLOB
$s = oci_parse($c, 'update xtt
                    set xt_spec = XMLType(:clob)
                    where xt_id = 1');
$lob = oci_new_descriptor($c, OCI_D_LOB);
oci_bind_by_name($s, ':clob', $lob, -1, OCI_B_CLOB);
$lob->writeTemporary($sx->asXml());
oci_execute($s);
$lob->close();

// Verify
echo "Verify\n";
$data = do_query($c);

// Cleanup

$stmtarray = array(
    "drop table xtt",
);

oci8_test_sql_execute($c, $stmtarray);

echo "Done\n";

?>
--EXPECT--
Initial Data
string(316) "<?xml version="1.0"?>
        <Xt>
          <XtId>1</XtId>
          <Size>Big</Size>
          <Area>12345</Area>
          <Hardness>20</Hardness>
          <Lip>Curved</Lip>
          <Color>Red</Color>
          <Nice>N</Nice>
          <Compact>Tiny</Compact>
          <Material>Steel</Material>
        </Xt>"
Verify
string(309) "<?xml version="1.0"?>
<Xt>
          <XtId>1</XtId>
          <Size>Big</Size>
          <Area>12345</Area>
          <Hardness>19</Hardness>
          <Lip>Curved</Lip>
          <Color>Red</Color>
          <Nice>Y</Nice>
          <Compact>Tiny</Compact>
          <Material>Steel</Material>
        </Xt>
"
Done
