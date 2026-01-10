--TEST--
SQLite3::createFunction use F ZPP for trampoline callback and does not leak
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        return strnatcmp(...$arguments);
    }
}
$o = new TrampolineTest();
$callback = [$o, 'NAT'];

var_dump($db->createCollation('', $callback));

try {
    var_dump($db->createCollation(new stdClass(), $callback));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($db->createCollation('NAT', $callback, new stdClass()));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Invalid SQLite3 object:\n";
$rc = new ReflectionClass(SQLite3::class);
$obj = $rc->newInstanceWithoutConstructor();

try {
    var_dump($obj->createCollation('NAT', $callback));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

var_dump($db->createCollation('NAT', $callback));

?>
--EXPECT--
bool(false)
TypeError: SQLite3::createCollation(): Argument #1 ($name) must be of type string, stdClass given
ArgumentCountError: SQLite3::createCollation() expects exactly 2 arguments, 3 given
Invalid SQLite3 object:
Error: The SQLite3 object has not been correctly initialised or is already closed
bool(true)
