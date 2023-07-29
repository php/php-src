--TEST--
PostgreSQL escape functions
--EXTENSIONS--
pgsql
--SKIPIF--
<?php include("skipif.inc"); ?>
--FILE--
<?php

include 'config.inc';
define('FILE_NAME', __DIR__ . '/php.gif');

// pg_escape_string() test
$before = "ABC\\ABC\'";
$expect  = "ABC\\\\ABC\\'";
$expect2  = "ABC\\\\ABC\\\\''"; //the way escape string differs from PostgreSQL 9.0
$after = pg_escape_string($before);
if ($expect === $after || $expect2 === $after) {
    echo "pg_escape_string() is Ok\n";
}
else {
    echo "pg_escape_string() is NOT Ok\n";
    var_dump($before);
    var_dump($after);
    var_dump($expect);
}

// pg_escape_bytea() test
$before = "ABC\\ABC";
$expect  = "ABC\\\\\\\\ABC";
$after  = pg_escape_bytea($before);
if ($expect === $after) {
    echo "pg_escape_bytea() is Ok\n";
}
else {
    echo "pg_escape_byte() is NOT Ok\n";
    var_dump($before);
    var_dump($after);
    var_dump($expect);
}

// Test using database
$data = file_get_contents(FILE_NAME);
$db   = pg_connect($conn_str);

// Insert binary to DB
$escaped_data = pg_escape_bytea($db, $data);
pg_query($db, "DELETE FROM ".$table_name." WHERE num = 10000;");
$sql = "INSERT INTO ".$table_name." (num, bin) VALUES (10000, CAST ('".$escaped_data."' AS BYTEA));";
pg_query($db, $sql);

// Retrieve binary from DB
for ($i = 0; $i < 2; $i++) {
    $sql = "SELECT bin::bytea FROM ".$table_name." WHERE num = 10000";
    $result = pg_query($db, $sql);
    $row = pg_fetch_array($result, 0, PGSQL_ASSOC);

    if ($data === pg_unescape_bytea($row['bin'])) {
        echo "pg_escape_bytea() actually works with database\n";
        break;
    }
    elseif (!$i) {
        // Force bytea escaping and retry
        @pg_query($db, "SET bytea_output = 'escape'");
    }
    else {
        $result = pg_query($db, $sql);
        echo "pg_escape_bytea() is broken\n";
        break;
    }
}

// pg_escape_literal/pg_escape_identifier
$before = "ABC\\ABC\'";
$expect	 = " E'ABC\\\\ABC\\\\'''";
$after = pg_escape_literal($db, $before);
if ($expect === $after) {
    echo "pg_escape_literal() is Ok\n";
}
else {
    echo "pg_escape_literal() is NOT Ok\n";
    var_dump($before);
    var_dump($after);
    var_dump($expect);
}

$before = "ABC\\ABC\'";
$expect	 = "\"ABC\ABC\'\"";
$after = pg_escape_identifier($db, $before);
if ($expect === $after) {
    echo "pg_escape_identifier() is Ok\n";
}
else {
    echo "pg_escape_identifier() is NOT Ok\n";
    var_dump($before);
    var_dump($after);
    var_dump($expect);
}

?>
--EXPECTF--
pg_escape_string() is Ok
pg_escape_bytea() is Ok
pg_escape_bytea() actually works with database
pg_escape_literal() is Ok
pg_escape_identifier() is Ok
