--TEST--
DB::parseDSN test
--SKIPIF--
<?php if (!include("DB.php")) print "skip"; ?>
--FILE--
<?php // -*- C++ -*-

// Test for: DB.php
// Parts tested: DB_Error, DB_Warning

require_once "DB.php";

function test($dsn) {
    print implode(",", DB::parseDSN($dsn))."\n";
}

print "testing DB::parseDSN...\n";
test("mysql");
test("odbc(mssql)");
test("mysql://localhost");
test("mysql://remote.host.com/db");
test("mysql://testuser:testpw");
test("oci8://user:pass@tns-name");
test("odbc(solid)://foo:bar@tcp+localhost+1313");

/* phptype,dbsyntax,protocol,hostspec,database,username,password */
?>
--GET--
--POST--
--EXPECT--
testing DB::parseDSN...
mysql,,,,,,
odbc,mssql,,,,,
mysql,mysql,,localhost,,,
mysql,mysql,,remote.host.com,db,,
mysql,mysql,,,,testuser,testpw
oci8,oci8,,tns-name,,user,pass
odbc,solid,tcp,localhost 1313,,foo,bar
