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
        return strtoupper($arguments[0]);
    }
}
$o = new TrampolineTest();
$callback = [$o, 'strtoupper'];

var_dump($db->createfunction('', $callback));

try {
    var_dump($db->createfunction(new stdClass(), $callback, new stdClass()));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($db->createfunction('strtoupper', $callback, new stdClass()));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Invalid SQLite3 object:\n";
$rc = new ReflectionClass(SQLite3::class);
$obj = $rc->newInstanceWithoutConstructor();

try {
    var_dump($obj->createfunction('strtoupper', $callback));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

var_dump($db->createfunction('strtoupper', $callback));

?>
--EXPECT--
bool(false)
TypeError: SQLite3::createFunction(): Argument #1 ($name) must be of type string, stdClass given
TypeError: SQLite3::createFunction(): Argument #3 ($argCount) must be of type int, stdClass given
Invalid SQLite3 object:
Error: The SQLite3 object has not been correctly initialised or is already closed
bool(true)
