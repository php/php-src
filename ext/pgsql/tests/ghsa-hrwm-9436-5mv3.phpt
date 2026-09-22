--TEST--
#GHSA-hrwm-9436-5mv3: pgsql extension does not check for errors during escaping
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("inc/skipif.inc"); ?>
--FILE--
<?php

include 'inc/config.inc';
define('FILE_NAME', __DIR__ . '/php.gif');

$db = pg_connect($conn_str);
pg_query($db, "DROP TABLE IF EXISTS ghsa_hrmw_9436_5mv3");
pg_query($db, "CREATE TABLE ghsa_hrmw_9436_5mv3 (bar text);");

// pg_escape_literal/pg_escape_identifier

$invalid = "ABC\xff\x30';";
$flags = PGSQL_DML_NO_CONV | PGSQL_DML_ESCAPE;

var_dump(pg_insert($db, $invalid, ['bar' => 'test'])); // table name str escape in php_pgsql_meta_data
var_dump(pg_insert($db, "$invalid.tbl", ['bar' => 'test'])); // schema name str escape in php_pgsql_meta_data
var_dump(pg_insert($db, 'ghsa_hrmw_9436_5mv3', ['bar' => $invalid])); // converted value str escape in php_pgsql_convert
var_dump(pg_insert($db, $invalid, [])); // ident escape in build_tablename
var_dump(pg_insert($db, 'ghsa_hrmw_9436_5mv3', [$invalid => 'foo'], $flags)); // ident escape for field php_pgsql_insert
var_dump(pg_insert($db, 'ghsa_hrmw_9436_5mv3', ['bar' => $invalid], $flags)); // str escape for field value in php_pgsql_insert
var_dump(pg_update($db, 'ghsa_hrmw_9436_5mv3', ['bar' => 'val'], [$invalid => 'test'], $flags)); // ident escape in build_assignment_string
var_dump(pg_update($db, 'ghsa_hrmw_9436_5mv3', ['bar' => 'val'], ['bar' => $invalid], $flags)); // invalid str escape in build_assignment_string
var_dump(pg_escape_literal($db, $invalid)); // pg_escape_literal escape
var_dump(pg_escape_identifier($db, $invalid)); // pg_escape_identifier escape

?>
--EXPECTF--

Warning: pg_insert(): Escaping table name 'ABC%s';' failed in %s on line %d
bool(false)

Warning: pg_insert(): Escaping table namespace 'ABC%s';.tbl' failed in %s on line %d
bool(false)

Notice: pg_insert(): String value escaping failed for PostgreSQL 'text' (bar) in %s on line %d
bool(false)

Notice: pg_insert(): Failed to escape table name 'ABC%s';' in %s on line %d
bool(false)

Notice: pg_insert(): Failed to escape field 'ABC%s';' in %s on line %d
bool(false)

Notice: pg_insert(): Failed to escape field 'bar' value in %s on line %d
bool(false)

Notice: pg_update(): Failed to escape field 'ABC%s';' in %s on line %d
bool(false)

Notice: pg_update(): Failed to escape field 'bar' value in %s on line %d
bool(false)

Warning: pg_escape_literal(): Failed to escape in %s on line %d
bool(false)

Warning: pg_escape_identifier(): Failed to escape in %s on line %d
bool(false)
