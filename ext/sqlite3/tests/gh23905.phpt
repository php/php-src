--TEST--
GH-23905 (Failed seek on SQLite3 blob streams sets the stream position to -1)
--EXTENSIONS--
sqlite3
--FILE--
<?php
require_once __DIR__ . '/new_db.inc';

$db->exec('CREATE TABLE test (id INTEGER PRIMARY KEY, data BLOB)');
$db->exec("INSERT INTO test (id, data) VALUES (1, x'68656c6c6f20776f726c64')"); // "hello world"

foreach (['read-only' => SQLITE3_OPEN_READONLY, 'read-write' => SQLITE3_OPEN_READWRITE] as $mode => $flags) {
    echo $mode, "\n";
    $stream = $db->openBlob('test', 'data', 1, 'main', $flags);

    echo "-- SEEK_SET --\n";
    fseek($stream, 6);
    var_dump(fseek($stream, -1, SEEK_SET), ftell($stream));
    var_dump(fseek($stream, -6, SEEK_CUR), ftell($stream), fread($stream, 5));
    var_dump(fseek($stream, -1, SEEK_SET), ftell($stream));
    var_dump(fseek($stream, 12, SEEK_SET), feof($stream), ftell($stream));
    var_dump(fread($stream, 10), feof($stream));

    echo "-- SEEK_CUR --\n";
    fseek($stream, 6);
    var_dump(fseek($stream, -7, SEEK_CUR), ftell($stream));
    var_dump(fseek($stream, -6, SEEK_CUR), ftell($stream), fread($stream, 5));
    var_dump(fseek($stream, -6, SEEK_CUR), ftell($stream));
    var_dump(fseek($stream, 7, SEEK_CUR), feof($stream), ftell($stream));
    var_dump(fread($stream, 10), feof($stream));

    echo "-- SEEK_END --\n";
    fseek($stream, 6);
    var_dump(fseek($stream, -12, SEEK_END), ftell($stream));
    var_dump(fseek($stream, -6, SEEK_CUR), ftell($stream), fread($stream, 5));
    var_dump(fseek($stream, -12, SEEK_END), ftell($stream));
    var_dump(fseek($stream, 1, SEEK_END), feof($stream), ftell($stream));
    var_dump(fread($stream, 10), feof($stream));

    if ($flags === SQLITE3_OPEN_READWRITE) {
        // A blob cannot grow, so a write after a failed seek past the end
        // writes at the unchanged position
        echo "-- write after failed seek --\n";
        fseek($stream, 6);
        var_dump(fseek($stream, 12, SEEK_SET), fwrite($stream, 'W'), ftell($stream));
        fseek($stream, 0);
        var_dump(fread($stream, 5));
        var_dump(fseek($stream, 1, SEEK_END), fwrite($stream, '!'), ftell($stream));
        var_dump(fseek($stream, 0), fread($stream, 15));
    }

    fclose($stream);
    echo "\n";
}

$db->close();
?>
--EXPECT--
read-only
-- SEEK_SET --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(-1)
int(5)
int(-1)
bool(false)
int(5)
string(6) " world"
bool(true)
-- SEEK_CUR --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(-1)
int(5)
int(-1)
bool(false)
int(5)
string(6) " world"
bool(true)
-- SEEK_END --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(-1)
int(5)
int(-1)
bool(false)
int(5)
string(6) " world"
bool(true)

read-write
-- SEEK_SET --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(-1)
int(5)
int(-1)
bool(false)
int(5)
string(6) " world"
bool(true)
-- SEEK_CUR --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(-1)
int(5)
int(-1)
bool(false)
int(5)
string(6) " world"
bool(true)
-- SEEK_END --
int(-1)
int(6)
int(0)
int(0)
string(5) "hello"
int(-1)
int(5)
int(-1)
bool(false)
int(5)
string(6) " world"
bool(true)
-- write after failed seek --
int(-1)
int(1)
int(7)
string(5) "hello"
int(-1)
int(1)
int(6)
int(0)
string(11) "hello!World"
