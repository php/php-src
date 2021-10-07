--TEST--
PDO PgSQL Bug #33876 (PDO misquotes/miscasts bool(false))
--SKIPIF--
<?php
if (!extension_loaded('pdo') || !extension_loaded('pdo_pgsql')) die('skip not loaded');
require __DIR__ . '/config.inc';
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
PDOTest::skip();
?>
--FILE--
<?php
require __DIR__ . '/../../../ext/pdo/tests/pdo_test.inc';
$db = PDOTest::test_factory(__DIR__ . '/common.phpt');
$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_SILENT);

$db->exec("SET LC_MESSAGES='C'");
$db->exec('CREATE TABLE test (foo varchar(5) NOT NULL, bar bool NOT NULL)');
$db->exec("INSERT INTO test VALUES('false','f')");
$db->exec("INSERT INTO test VALUES('true', 't')");

$res = $db->prepare('SELECT foo from test where bar = ?');

# this is the portable approach to binding a bool
$res->bindValue(1, false, PDO::PARAM_BOOL);
if (!$res->execute())
	print_r($res->errorInfo());
else
	print_r($res->fetchAll(PDO::FETCH_ASSOC));

# this is the portable approach to binding a bool
$res->bindValue(1, true, PDO::PARAM_BOOL);
if (!$res->execute())
	print_r($res->errorInfo());
else
	print_r($res->fetchAll(PDO::FETCH_ASSOC));


# true gets cast to string (because the implied default is string)
# true-as-string is 1, so this "works"
if (!$res->execute(array(true)))
	print_r($res->errorInfo());
else
	print_r($res->fetchAll(PDO::FETCH_ASSOC));

# Expected to fail; unless told otherwise, PDO assumes string inputs
# false -> "" as string, which pgsql doesn't like
if (!$res->execute(array(false)))
    print_r(normalizeErrorInfo($res->errorInfo()));
else
	print_r($res->fetchAll(PDO::FETCH_ASSOC));

# And now using emulator prepares
echo "EMUL\n";


$res = $db->prepare('SELECT foo from test where bar = ?', array(
	PDO::ATTR_EMULATE_PREPARES => true));

# this is the portable approach to binding a bool
$res->bindValue(1, false, PDO::PARAM_BOOL);
if (!$res->execute())
	print_r($res->errorInfo());
else
	print_r($res->fetchAll(PDO::FETCH_ASSOC));

# this is the portable approach to binding a bool
$res->bindValue(1, true, PDO::PARAM_BOOL);
if (!$res->execute())
	print_r($res->errorInfo());
else
	print_r($res->fetchAll(PDO::FETCH_ASSOC));


# true gets cast to string (because the implied default is string)
# true-as-string is 1, so this "works"
if (!$res->execute(array(true)))
	print_r($res->errorInfo());
else
	print_r($res->fetchAll(PDO::FETCH_ASSOC));

# Expected to fail; unless told otherwise, PDO assumes string inputs
# false -> "" as string, which pgsql doesn't like
if (!$res->execute(array(false))) {
    print_r(normalizeErrorInfo($res->errorInfo()));
} else {
	print_r($res->fetchAll(PDO::FETCH_ASSOC));
}

function normalizeErrorInfo(array $err): array {
    // Strip additional lines outputted by recent PgSQL versions
    $err[2] = trim(current(explode("\n", $err[2])));
    return $err;
}

?>
--EXPECTF--
Array
(
    [0] => Array
        (
            [foo] => false
        )

)
Array
(
    [0] => Array
        (
            [foo] => true
        )

)
Array
(
    [0] => Array
        (
            [foo] => true
        )

)
Array
(
    [0] => 22P02
    [1] => 7
    [2] => %s:  %sboolean%s
)
EMUL
Array
(
    [0] => Array
        (
            [foo] => false
        )

)
Array
(
    [0] => Array
        (
            [foo] => true
        )

)
Array
(
    [0] => Array
        (
            [foo] => true
        )

)
Array
(
    [0] => 22P02
    [1] => 7
    [2] => %s:  %sboolean%s
)
