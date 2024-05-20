--TEST--
Test PdoSqlite::createFunction() arguments error
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

declare(strict_types=1);

$db = new PdoSqlite('sqlite::memory:');

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        return '';
    }
}

try {
    $db->createFunction(null, [new TrampolineTest(), 'strtoupper']);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

try {
    $db->createFunction('strtoupper', null);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

try {
    $db->createFunction('strtoupper', [new TrampolineTest(), 'strtoupper'], null);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

try {
    $db->createFunction('strtoupper', [new TrampolineTest(), 'strtoupper'], 1, null);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

echo 'done!';
?>
--EXPECT--
PdoSqlite::createFunction(): Argument #1 ($function_name) must be of type string, null given
PdoSqlite::createFunction(): Argument #2 ($callback) must be a valid callback, no array or string given
PdoSqlite::createFunction(): Argument #3 ($num_args) must be of type int, null given
PdoSqlite::createFunction(): Argument #4 ($flags) must be of type int, null given
done!
