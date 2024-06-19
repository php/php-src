--TEST--
Test Pdo\Sqlite::createAggregate() arguments error
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

declare(strict_types=1);

$db = new Pdo\Sqlite('sqlite::memory:');

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        return '';
    }
}

try {
    $db->createAggregate(null, [new TrampolineTest(), 'step'], null, 1);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

try {
    $db->createAggregate(null, null, [new TrampolineTest(), 'step'], 1);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

try {
    $db->createAggregate(null, [new TrampolineTest(), 'step'], [new TrampolineTest(), 'step'], 1);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

try {
    $db->createAggregate('S', null, [new TrampolineTest(), 'finalize'], 1);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

try {
    $db->createAggregate('S', [new TrampolineTest(), 'step'], null, 1);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

try {
    $db->createAggregate('S', [new TrampolineTest(), 'step'], [new TrampolineTest(), 'finalize'], null);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

echo 'done!';
?>
--EXPECT--
Pdo\Sqlite::createAggregate(): Argument #1 ($name) must be of type string, null given
Pdo\Sqlite::createAggregate(): Argument #1 ($name) must be of type string, null given
Pdo\Sqlite::createAggregate(): Argument #1 ($name) must be of type string, null given
Pdo\Sqlite::createAggregate(): Argument #2 ($step) must be a valid callback, no array or string given
Pdo\Sqlite::createAggregate(): Argument #3 ($finalize) must be a valid callback, no array or string given
Pdo\Sqlite::createAggregate(): Argument #4 ($numArgs) must be of type int, null given
done!
