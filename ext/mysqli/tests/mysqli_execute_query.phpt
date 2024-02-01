--TEST--
mysqli_execute_query()
--EXTENSIONS--
mysqli
--SKIPIF--
<?php
require_once 'skipifconnectfailure.inc';
?>
--FILE--
<?php

require 'table.inc';

if (!($tmp = mysqli_execute_query($link, "SELECT id, label FROM test ORDER BY id"))) {
    printf("[001] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

if (!is_a($tmp, mysqli_result::class)) {
    printf("[002] Expecting mysqli_result, got %s/%s\n", gettype($tmp), $tmp);
}

unset($tmp);

// procedural
if (!($tmp = mysqli_execute_query($link, "SELECT ? AS a, ? AS b, ? AS c", [42, "foo", null]))) {
    printf("[003] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

assert($tmp->fetch_assoc() === ['a' => '42', 'b' => 'foo', 'c' => null]);

// OO style
if (!($tmp = $link->execute_query("SELECT ? AS a, ? AS b, ? AS c", [42, "foo", null]))) {
    printf("[004] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

assert($tmp->fetch_assoc() === ['a' => '42', 'b' => 'foo', 'c' => null]);

// prepare error
if (!($tmp = $link->execute_query("some random gibberish", [1, "foo"]))) {
    printf("[005] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

// stmt error - duplicate key
if (!$link->execute_query("INSERT INTO test(id, label) VALUES (?, ?)", [1, "foo"])) {
    printf("[006] [%d] %s\n", mysqli_errno($link), mysqli_error($link));
}

// successful update returns true
if (!($tmp = $link->execute_query("UPDATE test SET label=? WHERE id=?", ["baz", 1]))) {
    printf("[007] Expecting true, got %s/%s\n", gettype($tmp), $tmp);
}
if ($link->affected_rows <= 0) {
    printf("[008] Expecting positive non-zero integer for affected_rows, got %s/%s\n", gettype($link->affected_rows), $link->affected_rows);
}

mysqli_report(MYSQLI_REPORT_ERROR | MYSQLI_REPORT_STRICT);

// check if value error is properly reported
try {
    $link->execute_query('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?', ['foo', 42]);
} catch (ValueError $e) {
    echo '[009] '.$e->getMessage()."\n";
}
try {
    $link->execute_query('SELECT label, ? AS anon, ? AS num FROM test WHERE id=?', ['foo' => 42]);
} catch (ValueError $e) {
    echo '[010] '.$e->getMessage()."\n";
}

// check if insert_id is copied
$link->execute_query("ALTER TABLE test MODIFY id INT NOT NULL AUTO_INCREMENT");
$link->execute_query("INSERT INTO test(label) VALUES (?)", ["foo"]);
if ($link->insert_id <= 0) {
    printf("[011] Expecting positive non-zero integer for insert_id, got %s/%s\n", gettype($link->insert_id), $link->insert_id);
}

// bad index
mysqli_report(MYSQLI_REPORT_ALL);
try {
    $link->execute_query("SELECT id FROM test WHERE label = ?", ["foo"]);
} catch (mysqli_sql_exception $e) {
    echo '[012] '.$e->getMessage()."\n";
}

print "done!";
?>
--CLEAN--
<?php
require_once "clean_table.inc";
?>
--EXPECTF--
[005] [1064] You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'some random gibberish' at line 1
[006] [1062] Duplicate entry '1' for key '%s'
[009] mysqli::execute_query(): Argument #2 ($params) must consist of exactly 3 elements, 2 present
[010] mysqli::execute_query(): Argument #2 ($params) must be a list array
[012] No index used in query/prepared statement SELECT id FROM test WHERE label = ?
done!
