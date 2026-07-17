--TEST--
SQLite3::escapeString() rejects strings with embedded NUL bytes
--EXTENSIONS--
sqlite3
--FILE--
<?php

$cases = [
    "\0",
    "a\0b",
    "secret\0x",
    "foo\0\0bar",
];

foreach ($cases as $in) {
    try {
        SQLite3::escapeString($in);
        echo "FAIL: no exception for ", bin2hex($in), "\n";
    } catch (ValueError $e) {
        echo "ValueError: ", $e->getMessage(), "\n";
    }
}

echo "ok: ", SQLite3::escapeString("ok"), "\n";
echo "quote: ", SQLite3::escapeString("test''%"), "\n";
echo "empty: ", var_export(SQLite3::escapeString(""), true), "\n";
?>
--EXPECT--
ValueError: SQLite3::escapeString(): Argument #1 ($string) must not contain any null bytes
ValueError: SQLite3::escapeString(): Argument #1 ($string) must not contain any null bytes
ValueError: SQLite3::escapeString(): Argument #1 ($string) must not contain any null bytes
ValueError: SQLite3::escapeString(): Argument #1 ($string) must not contain any null bytes
ok: ok
quote: test''''%
empty: ''
