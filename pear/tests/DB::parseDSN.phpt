--TEST--
DB::parseDSN test
--SKIPIF--
<?php if (!include("DB.php")) print "skip"; ?>
--FILE--
<?php // -*- C++ -*-

// Test for: DB.php
// Parts tested: DB_Error, DB_Warning

require_once "DB.php";

print "testing DB::parseDSN...\n";
var_dump(DB::parseDSN("mysql"));
var_dump(DB::parseDSN("odbc(mssql)"));
var_dump(DB::parseDSN("mysql://localhost"));
var_dump(DB::parseDSN("mysql://remote.host.com/db"));
var_dump(DB::parseDSN("mysql://testuser:testpw"));
var_dump(DB::parseDSN("oci8://user:pass@tns-name"));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));
var_dump(DB::parseDSN(""));

?>
--GET--
--POST--
--EXPECT--
testing ibase: object: db_ibase: (phptype=ibase, dbsyntax=ibase)
testing msql: object: db_msql: (phptype=msql, dbsyntax=msql)
testing mssql: object: db_mssql: (phptype=mssql, dbsyntax=mssql)
testing mysql: object: db_mysql: (phptype=mysql, dbsyntax=mysql)
testing oci8: object: db_oci8: (phptype=oci8, dbsyntax=oci8)
testing odbc: object: db_odbc: (phptype=odbc, dbsyntax=unknown)
testing pgsql: object: db_pgsql: (phptype=pgsql, dbsyntax=pgsql)
testing sybase: object: db_sybase: (phptype=sybase, dbsyntax=sybase)
