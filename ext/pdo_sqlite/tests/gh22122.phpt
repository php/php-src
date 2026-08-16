--TEST--
GH-22122 (Use-after-free in Pdo\Sqlite authorizer when callback releases the authorizer)
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
$db = Pdo\Sqlite::connect('sqlite::memory:');

class Auth {
    public string $state = "alive";

    public function authorize(int $action, ...$args): int {
        global $db;
        $db->setAuthorizer(null);
        echo "method: ", $this->state, "\n";
        return Pdo\Sqlite::OK;
    }
}
$auth = new Auth();
$db->setAuthorizer([$auth, 'authorize']);
unset($auth);
$db->exec('SELECT 1');

$capture = "closure-alive";
$closure = function (int $action, ...$args) use (&$capture, $db): int {
    $db->setAuthorizer(null);
    echo "closure: ", $capture, "\n";
    return Pdo\Sqlite::OK;
};
$db->setAuthorizer($closure);
unset($closure);
$db->exec('SELECT 2');

$db->exec('SELECT 3');
echo "post-disable query ok\n";
?>
--EXPECT--
method: alive
closure: closure-alive
post-disable query ok
