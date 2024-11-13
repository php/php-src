--TEST--
SQLite3::setAuthorizer use F ZPP for trampoline callback and does not leak
--EXTENSIONS--
sqlite3
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        if ($arguments[0] == SQLite3::SELECT) {
            return SQLite3::OK;
        }

        return SQLite3::DENY;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'authorizer'];

$db = new SQLite3(':memory:');
$db->enableExceptions(true);

echo "Invalid SQLite3 object:\n";
$rc = new ReflectionClass(SQLite3::class);
$obj = $rc->newInstanceWithoutConstructor();

try {
    var_dump($obj->setAuthorizer($callback));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$db->setAuthorizer($callback);

?>
DONE
--EXPECT--
Invalid SQLite3 object:
Error: The SQLite3 object has not been correctly initialised or is already closed
DONE
