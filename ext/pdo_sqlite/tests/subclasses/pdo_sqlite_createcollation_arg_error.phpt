--TEST--
Test Pdo\Sqlite::createCollation() arguments error
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
    $db->createCollation(null, [new TrampolineTest(), 'NAT']);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

try {
    $db->createCollation('NAT', null);
} catch (Throwable $e) {
    echo $e->getMessage() . "\n";
}

echo 'done!';
?>
--EXPECT--
Pdo\Sqlite::createCollation(): Argument #1 ($name) must be of type string, null given
Pdo\Sqlite::createCollation(): Argument #2 ($callback) must be a valid callback, no array or string given
done!
