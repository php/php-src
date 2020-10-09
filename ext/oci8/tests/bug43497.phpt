--TEST--
Bug #43497 (OCI8 XML/getClobVal aka temporary LOBs leak UGA memory)
--SKIPIF--
<?php
$target_dbs = array('oracledb' => true, 'timesten' => false);  // test runs on these DBs
require(__DIR__.'/skipif.inc');
if (getenv('SKIP_SLOW_TESTS')) die('skip slow tests excluded by request');
?>
--FILE--
<?php

require __DIR__.'/connect.inc';

function sessionid($c)  // determines and returns current session ID
{
    $query = "select sid from v\$session where audsid = userenv('sessionid')";

    $stmt = oci_parse($c, $query);

    if (oci_execute($stmt, OCI_DEFAULT)) {
        $row = oci_fetch($stmt);
        return oci_result($stmt, 1);
    }

    return null;
}


function templobs($c, $sid)  // returns number of temporary LOBs
{
    $query = "select abstract_lobs from v\$temporary_lobs where sid = " . $sid;

    $stmt = oci_parse($c, $query);

    if (oci_execute($stmt, OCI_DEFAULT)) {
        $row = oci_fetch($stmt);
        $val = oci_result($stmt, 1);
        oci_free_statement($stmt);
        return $val;
    }
    return null;
}


// Read all XML data using explicit LOB locator
function readxmltab_ex($c)
{
    $stmt = oci_parse($c, "select extract(xml, '/').getclobval() from bug43497_tab");

    $cntchk = 0;
    if (oci_execute($stmt)) {
        while ($result = oci_fetch_array($stmt, OCI_NUM)) {
            $result[0]->free();   // cleanup properly
            ++$cntchk;
        }
    }
    echo "Loop count check = $cntchk\n";
}

// Read all XML data using explicit LOB locator but without freeing the temp lobs
function readxmltab_ex_nofree($c)
{
    $stmt = oci_parse($c, "select extract(xml, '/').getclobval() from bug43497_tab");

    $cntchk = 0;
    if (oci_execute($stmt)) {
        while ($result = oci_fetch_array($stmt, OCI_NUM)) {
            ++$cntchk;
        }
    }
    echo "Loop count check = $cntchk\n";
}

// Read all XML data using implicit LOB locator
function readxmltab_im($c)
{
    $stmt = oci_parse($c, "select extract(xml, '/').getclobval() from bug43497_tab");

    $cntchk = 0;
    if (oci_execute($stmt)) {
        while ($result = oci_fetch_array($stmt, OCI_NUM+OCI_RETURN_LOBS)) {
            ++$cntchk;
        }
    }
    echo "Loop count check = $cntchk\n";
}

function createxmltab($c)  // create table w/ field of XML type
{
    @dropxmltab($c);
    $stmt = oci_parse($c, "create table bug43497_tab (id number primary key, xml xmltype)");
    oci_execute($stmt);
}

function dropxmltab($c)  // delete table
{
    $stmt = oci_parse($c, "drop table bug43497_tab");
    oci_execute($stmt);
}


function fillxmltab($c)
{
    for ($id = 1; $id <= 100; $id++) {

        // create an XML element string with random data
        $s = "<data>";
        for ($j = 0; $j < 128; $j++) {
            $s .= rand();
        }
        $s .= "</data>\n";
        for ($j = 0; $j < 4; $j++) {
            $s .= $s;
        }
        $data = "<?xml version=\"1.0\"?><records>" . $s . "</records>";

        // insert XML data into database

        $stmt = oci_parse($c, "insert into bug43497_tab(id, xml) values (:id, sys.xmltype.createxml(:xml))");
        oci_bind_by_name($stmt, ":id", $id);
        $clob = oci_new_descriptor($c, OCI_D_LOB);
        oci_bind_by_name($stmt, ":xml", $clob, -1, OCI_B_CLOB);
        $clob->writetemporary($data);
        oci_execute($stmt);

        $clob->close();
        $clob->free();
    }
}


// Initialize

createxmltab($c);
fillxmltab($c);

// Run Test

$sid = sessionid($c);

echo "Explicit LOB use\n";
for ($i = 1; $i <= 10; $i++) {
    echo "\nRun              = " . $i . "\n";
    echo "Temporary LOBs   = " . templobs($c, $sid) . "\n";
    readxmltab_ex($c);
}

echo "\nImplicit LOB use\n";
for ($i = 1; $i <= 10; $i++) {
    echo "\nRun              = " . $i . "\n";
    echo "Temporary LOBs   = " . templobs($c, $sid) . "\n";
    readxmltab_im($c);
}

echo "\nExplicit LOB with no free\n";
for ($i = 1; $i <= 10; $i++) {
    echo "\nRun              = " . $i . "\n";
    echo "Temporary LOBs   = " . templobs($c, $sid) . "\n";
    readxmltab_ex_nofree($c);
}



// Cleanup

dropxmltab($c);

oci_close($c);

echo "Done\n";
?>
--EXPECT--
Explicit LOB use

Run              = 1
Temporary LOBs   = 0
Loop count check = 100

Run              = 2
Temporary LOBs   = 0
Loop count check = 100

Run              = 3
Temporary LOBs   = 0
Loop count check = 100

Run              = 4
Temporary LOBs   = 0
Loop count check = 100

Run              = 5
Temporary LOBs   = 0
Loop count check = 100

Run              = 6
Temporary LOBs   = 0
Loop count check = 100

Run              = 7
Temporary LOBs   = 0
Loop count check = 100

Run              = 8
Temporary LOBs   = 0
Loop count check = 100

Run              = 9
Temporary LOBs   = 0
Loop count check = 100

Run              = 10
Temporary LOBs   = 0
Loop count check = 100

Implicit LOB use

Run              = 1
Temporary LOBs   = 0
Loop count check = 100

Run              = 2
Temporary LOBs   = 0
Loop count check = 100

Run              = 3
Temporary LOBs   = 0
Loop count check = 100

Run              = 4
Temporary LOBs   = 0
Loop count check = 100

Run              = 5
Temporary LOBs   = 0
Loop count check = 100

Run              = 6
Temporary LOBs   = 0
Loop count check = 100

Run              = 7
Temporary LOBs   = 0
Loop count check = 100

Run              = 8
Temporary LOBs   = 0
Loop count check = 100

Run              = 9
Temporary LOBs   = 0
Loop count check = 100

Run              = 10
Temporary LOBs   = 0
Loop count check = 100

Explicit LOB with no free

Run              = 1
Temporary LOBs   = 0
Loop count check = 100

Run              = 2
Temporary LOBs   = 0
Loop count check = 100

Run              = 3
Temporary LOBs   = 0
Loop count check = 100

Run              = 4
Temporary LOBs   = 0
Loop count check = 100

Run              = 5
Temporary LOBs   = 0
Loop count check = 100

Run              = 6
Temporary LOBs   = 0
Loop count check = 100

Run              = 7
Temporary LOBs   = 0
Loop count check = 100

Run              = 8
Temporary LOBs   = 0
Loop count check = 100

Run              = 9
Temporary LOBs   = 0
Loop count check = 100

Run              = 10
Temporary LOBs   = 0
Loop count check = 100
Done
