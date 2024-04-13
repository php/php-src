--TEST--
GH-13952 (sqlite PDO::quote silently corrupts strings with null bytes)
--EXTENSIONS--
pdo
pdo_sqlite
--FILE--
<?php
$db = new \PDO('sqlite::memory:', null, null, array(
    \PDO::ATTR_ERRMODE => \PDO::ERRMODE_EXCEPTION,
    \PDO::ATTR_DEFAULT_FETCH_MODE => \PDO::FETCH_ASSOC,
    \PDO::ATTR_EMULATE_PREPARES => false,
));

$test_cases = [
    "",
    "x",
    "\x00",
    "a\x00b",
    "\x00\x00\x00",
    "foobar",
    "foo'''bar",
    "'foo'''bar'",
    "'foo'\x00'bar'",
    "foo\x00\x00\x00bar",
    "\x00foo\x00\x00\x00bar\x00",
    "\x00\x00\x00foo",
    "foo\x00\x00\x00",
];

$db->exec('CREATE TABLE test (name TEXT)');

foreach ($test_cases as $test_case) {
    $quoted = $db->quote($test_case);
    echo trim(json_encode($test_case), '"'), " -> $quoted\n";
    $db->exec("INSERT INTO test (name) VALUES (" . $quoted . ")");
}

$stmt = $db->prepare('SELECT * from test');
$stmt->execute();
foreach ($stmt->fetchAll() as $result) {
    var_dump($result['name']);
}
?>
--EXPECTF--
-> ''
x -> 'x'
\u0000 -> x'00'
a\u0000b -> 'a'||x'00'||'b'
\u0000\u0000\u0000 -> x'000000'
foobar -> 'foobar'
foo'''bar -> 'foo''''''bar'
'foo'''bar' -> '''foo''''''bar'''
'foo'\u0000'bar' -> '''foo'''||x'00'||'''bar'''
foo\u0000\u0000\u0000bar -> 'foo'||x'000000'||'bar'
\u0000foo\u0000\u0000\u0000bar\u0000 -> x'00'||'foo'||x'000000'||'bar'||x'00'
\u0000\u0000\u0000foo -> x'000000'||'foo'
foo\u0000\u0000\u0000 -> 'foo'||x'000000'
string(0) ""
string(1) "x"
string(1) "%0"
string(3) "a%0b"
string(3) "%0%0%0"
string(6) "foobar"
string(9) "foo'''bar"
string(11) "'foo'''bar'"
string(11) "'foo'%0'bar'"
string(9) "foo%0%0%0bar"
string(11) "%0foo%0%0%0bar%0"
string(6) "%0%0%0foo"
string(6) "foo%0%0%0"
