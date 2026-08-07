--TEST--
GHSA-7qpv-r5mr-78m4: SQL injection via E'...' backslash breakout
--CREDITS--
expatch.llc
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php
include 'inc/config.inc';

$db = pg_connect($conn_str);

pg_query($db, "SET standard_conforming_strings = 1");

pg_query($db, "DROP TABLE IF EXISTS ghsa_7qpv_r5mr_78m4");
pg_query($db, "CREATE TABLE ghsa_7qpv_r5mr_78m4 (id serial primary key, name text, admin boolean)");
pg_query($db, "INSERT INTO ghsa_7qpv_r5mr_78m4 (name, admin) VALUES ('alice', false), ('bob', false)");

$params = ['name' => "zzz' OR 1=1 --"];
echo pg_select($db, 'ghsa_7qpv_r5mr_78m4', $params, PGSQL_DML_STRING) . "\n";
printf("returned: %d\n\n", count(pg_select($db, 'ghsa_7qpv_r5mr_78m4', $params)));

$params = ['name' => "zzz\\' OR 1=1 --"];
echo pg_select($db, 'ghsa_7qpv_r5mr_78m4', $params, PGSQL_DML_STRING) . "\n";
printf("returned: %d\n\n", count(pg_select($db, 'ghsa_7qpv_r5mr_78m4', $params)));

$params = ['name' => "john\\', true) --", 'admin' => 'false'];
echo pg_insert($db, 'ghsa_7qpv_r5mr_78m4', $params, PGSQL_DML_STRING) . "\n";
pg_insert($db, 'ghsa_7qpv_r5mr_78m4', $params);
var_dump(pg_select($db, 'ghsa_7qpv_r5mr_78m4', ['id' => 3])[0]['admin']);
echo "\n";

$params = ['name' => "jake\\', true) --", 'admin' => 'f'];
echo pg_insert($db, 'ghsa_7qpv_r5mr_78m4', $params, PGSQL_DML_ESCAPE|PGSQL_DML_STRING) . "\n";
pg_insert($db, 'ghsa_7qpv_r5mr_78m4', $params, PGSQL_DML_EXEC|PGSQL_DML_ESCAPE);
var_dump(pg_select($db, 'ghsa_7qpv_r5mr_78m4', ['id' => 4])[0]['admin']);

?>
--EXPECT--
SELECT * FROM "ghsa_7qpv_r5mr_78m4" WHERE "name"='zzz'' OR 1=1 --';
returned: 0

SELECT * FROM "ghsa_7qpv_r5mr_78m4" WHERE "name"='zzz\'' OR 1=1 --';
returned: 0

INSERT INTO "ghsa_7qpv_r5mr_78m4" ("name","admin") VALUES ('john\'', true) --','f');
string(1) "f"

INSERT INTO "ghsa_7qpv_r5mr_78m4" ("name","admin") VALUES ('jake\'', true) --','f');
string(1) "f"
--CLEAN--
<?php
include('inc/config.inc');
$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS ghsa_7qpv_r5mr_78m4");
?>
