--TEST--
GH-13952 (sqlite PDO::quote handles null bytes correctly)
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php

$modes = [
    'exception' => PDO::ERRMODE_EXCEPTION,
    'warning'   => PDO::ERRMODE_WARNING,
    'silent'    => PDO::ERRMODE_SILENT,
];

$test_cases = [
    "",
    "x",
    "\x00",
    "a\x00b",
    "\x00\x00\x00",
    "foobar",
    "foo'''bar",
    "'foo'''bar'",
    "foo\x00bar",
    "'foo'\x00'bar'",
    "foo\x00\x00\x00bar",
    "\x00foo\x00\x00\x00bar\x00",
    "\x00\x00\x00foo",
    "foo\x00\x00\x00",
    "\x80", // << invalid UTF-8
    "\x00\x80\x00", // << invalid UTF-8 with null bytes
];

foreach ($modes as $mode_name => $mode) {
    echo "Testing error mode: $mode_name\n";
    $db = new PDO('sqlite::memory:', null, null, [PDO::ATTR_ERRMODE => $mode]);

    foreach ($test_cases as $test) {
        $contains_null = str_contains($test, "\x00");

        if ($mode === PDO::ERRMODE_EXCEPTION && $contains_null) {
            set_error_handler(fn() => throw new PDOException(), E_WARNING);
            try {
                $db->quote($test);
                throw new LogicException("Expected exception not thrown.");
            } catch (PDOException) {
                // expected
            } finally {
                restore_error_handler();
            }
        } else {
            set_error_handler(fn() => null, E_WARNING);
            $quoted = $db->quote($test);
            restore_error_handler();

            if ($contains_null) {
                if ($quoted !== false) {
                    throw new LogicException("Expected false, got: " . var_export($quoted, true));
                }
            } else {
                if ($quoted === false) {
                    throw new LogicException("Unexpected false from quote().");
                }
                $fetched = $db->query("SELECT $quoted")->fetchColumn();
                if ($fetched !== $test) {
                    throw new LogicException("Data corrupted: expected " . var_export($test, true) . " got " . var_export($fetched, true));
                }
            }
        }
    }
}

echo "ok\n";
?>
--EXPECT--
Testing error mode: exception
Testing error mode: warning
Testing error mode: silent
ok
