--TEST--
Pdo\Sqlite user authorizer trampoline callback
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php

class TrampolineTest {
    public function __call(string $name, array $arguments) {
        echo 'Trampoline for ', $name, PHP_EOL;
        if ($arguments[0] == 21 /* SELECT */) {
            return Pdo\Sqlite::OK;
        }

        return Pdo\Sqlite::DENY;
    }
}
$o = new TrampolineTest();
$callback = [$o, 'authorizer'];

$db = new Pdo\Sqlite('sqlite::memory:');

$db->setAuthorizer($callback);

// This query should be accepted
var_dump($db->query('SELECT 1;'));

try {
    // This one should fail
    var_dump($db->query('CREATE TABLE test (a, b);'));
} catch (\Exception $e) {
    echo $e->getMessage() . "\n";
}

?>
--EXPECTF--
Trampoline for authorizer
object(PDOStatement)#%d (1) {
  ["queryString"]=>
  string(9) "SELECT 1;"
}
Trampoline for authorizer
SQLSTATE[HY000]: General error: 23 not authorized
