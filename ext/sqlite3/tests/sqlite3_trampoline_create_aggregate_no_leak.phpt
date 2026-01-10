--TEST--
SQLite3::createAggregate() use F ZPP for trampoline callback and does not leak
--EXTENSIONS--
sqlite3
--FILE--
<?php

require_once(__DIR__ . '/new_db.inc');

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        $context = $arguments[0];
        if ($name === 'finalize') {
            return implode(',', $context['values']);
        }
        if (empty($context)) {
            $context = ['total' => 0, 'values' => []];
        }
        $context['total'] += (int) $arguments[2];
        $context['values'][] = $context['total'];
        return $context;
    }
}
$o = new TrampolineTest();
$step = [$o, 'step'];
$finalize = [$o, 'finalize'];

var_dump($db->createAggregate('', $step, $finalize, 1));

try {
    var_dump($db->createAggregate(new stdClass(), $step, $finalize, new stdClass()));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($db->createAggregate('S', $step, $finalize, new stdClass()));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($db->createAggregate('S', $step, 'no_func', 1));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($db->createAggregate('S', 'no_func', $finalize, 1));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    var_dump($db->createAggregate('S', $step, $finalize, 'not a number'));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

echo "Invalid SQLite3 object:\n";
$rc = new ReflectionClass(SQLite3::class);
$obj = $rc->newInstanceWithoutConstructor();

try {
    var_dump($obj->createAggregate('S', $step, $finalize, 1));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

var_dump($db->createAggregate('S', $step, $finalize, 1));

echo "Closing database\n";
var_dump($db->close());
echo "Done\n";
?>
--EXPECT--
bool(false)
TypeError: SQLite3::createAggregate(): Argument #1 ($name) must be of type string, stdClass given
TypeError: SQLite3::createAggregate(): Argument #4 ($argCount) must be of type int, stdClass given
TypeError: SQLite3::createAggregate(): Argument #3 ($finalCallback) must be a valid callback, function "no_func" not found or invalid function name
TypeError: SQLite3::createAggregate(): Argument #2 ($stepCallback) must be a valid callback, function "no_func" not found or invalid function name
TypeError: SQLite3::createAggregate(): Argument #4 ($argCount) must be of type int, string given
Invalid SQLite3 object:
Error: The SQLite3 object has not been correctly initialised or is already closed
bool(true)
Closing database
bool(true)
Done
