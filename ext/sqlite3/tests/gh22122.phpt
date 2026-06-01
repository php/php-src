--TEST--
GH-22122 (Use-after-free in SQLite3 authorizer when callback releases the authorizer)
--EXTENSIONS--
sqlite3
--FILE--
<?php
$db = new SQLite3(':memory:');

class Auth {
    public string $state = "alive";

    public function authorize(int $action, ...$args): int {
        global $db;
        $db->setAuthorizer(null);
        echo "method: ", $this->state, "\n";
        return SQLite3::OK;
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
    return SQLite3::OK;
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
