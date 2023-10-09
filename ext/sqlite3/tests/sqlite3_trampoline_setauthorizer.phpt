--TEST--
SQLite3 user authorizer trampoline callback
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

$db->setAuthorizer($callback);

// This query should be accepted
var_dump($db->querySingle('SELECT 1;'));

try {
    // This one should fail
    var_dump($db->querySingle('CREATE TABLE test (a, b);'));
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECT--
Trampoline for authorizer
int(1)
Trampoline for authorizer
Unable to prepare statement: not authorized
