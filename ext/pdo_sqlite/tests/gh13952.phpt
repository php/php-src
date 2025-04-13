--TEST--
GH-13952 (sqlite PDO::quote silently corrupts strings with null bytes)
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php
$db = new \PDO('sqlite::memory:', null, null, [
    \PDO::ATTR_ERRMODE            => \PDO::ERRMODE_EXCEPTION,
    \PDO::ATTR_DEFAULT_FETCH_MODE => \PDO::FETCH_ASSOC,
    \PDO::ATTR_EMULATE_PREPARES   => false,
]);

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
    "\x80",       // Invalid UTF-8 sequence
    "\x00\x80\x00", // Invalid UTF-8 sequence with null bytes
];

foreach ($test_cases as $test) {
    $should_throw = str_contains($test, "\x00");
    try {
        $quoted = $db->quote($test);
        if ($should_throw) {
            $displayTest = var_export($test, true);
            throw new LogicException("Failed for {$displayTest}: expected an exception but none was thrown.");
        }
    } catch (\PDOException $e) {
        if (!$should_throw) {
            $displayTest = var_export($test, true);
            throw new LogicException("Failed for {$displayTest}: unexpected exception thrown.", 0, $e);
        }
        // Exception is expected
        continue;
    }
    $fetched = $db->query("SELECT $quoted")->fetch($db::FETCH_NUM)[0];
    if ($fetched !== $test) {
        $displayTest = var_export($test, true);
        $displayFetched = var_export($fetched, true);
        throw new LogicException("Round-trip data corruption for {$displayTest}: got {$displayFetched}.");
    }
}
echo "ok\n";
?>
--EXPECT--
ok
