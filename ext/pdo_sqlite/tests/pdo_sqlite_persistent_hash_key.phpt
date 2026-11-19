--TEST--
PDO persistent hash key does not collide on colons in credentials
--EXTENSIONS--
pdo_sqlite
--FILE--
<?php
function persistent_shares(
    string $user1,
    string $pass1,
    string $user2,
    string $pass2,
    mixed $persist1,
    mixed $persist2,
    string $table,
): bool {
    $db1 = new PDO('sqlite::memory:', $user1, $pass1, [PDO::ATTR_PERSISTENT => $persist1]);
    $db1->exec("CREATE TABLE {$table} (id INT)");
    unset($db1);
    $db2 = new PDO('sqlite::memory:', $user2, $pass2, [PDO::ATTR_PERSISTENT => $persist2]);
    return (bool) $db2->query("SELECT 1 FROM sqlite_master WHERE name='{$table}'")->fetchColumn();
}

echo "user/pass colon collision: ";
var_dump(persistent_shares('admin', 's:ecret', 'admin:s', 'ecret', true, true, 't_userpass'));

echo "custom key colon collision: ";
var_dump(persistent_shares('cku', 'b', 'cku', 'b:c', 'c:d', 'd', 't_customkey'));

echo "same credentials reuse: ";
var_dump(persistent_shares('same', 'creds', 'same', 'creds', true, true, 't_same'));

echo "distinct credentials: ";
var_dump(persistent_shares('alice', 'secret', 'bob', 'secret', true, true, 't_distinct'));
?>
--EXPECT--
user/pass colon collision: bool(false)
custom key colon collision: bool(false)
same credentials reuse: bool(true)
distinct credentials: bool(false)
