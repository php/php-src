--TEST--
PdoSqlite::setAuthorizer use F ZPP for trampoline callback and does not leak
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        if ($arguments[0] == Pdo\Sqlite::SELECT) {
            return Pdo\Sqlite::OK;
        }

        return Pdo\Sqlite::DENY;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'authorizer'];

echo "Invalid Pdo\Sqlite object:\n";
$rc = new ReflectionClass(Pdo\Sqlite::class);
$obj = $rc->newInstanceWithoutConstructor();

try {
    var_dump($obj->setAuthorizer($callback));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
DONE
--EXPECT--
Invalid Pdo\Sqlite object:
Error: Pdo\Sqlite object is uninitialized
DONE
