--TEST--
PDO Common: PDO::FETCH_DEFAULT
--EXTENSIONS--
pdo
--SKIPIF--
<?php
$dir = getenv('REDIR_TEST_DIR');
if (false == $dir) die('skip no driver');
require_once $dir . 'pdo_test.inc';
PDOTest::skip();
?>
--CLEAN--
<?php
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();
PDOTest::dropTableIfExists($db, 'pdo_fetch_default');
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR=' . __DIR__ . '/../../pdo/tests/');
}
$table = 'pdo_fetch_default';
require_once getenv('REDIR_TEST_DIR') . "/pdo_fetch_setup.php";

$query = "SELECT name, country FROM {$table} WHERE userid = 104";
$expectedFetchMode = \PDO::FETCH_OBJ;
$db->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, $expectedFetchMode);

print "Original:\n";
$stmt = $db->query($query);
print_r($stmt->fetch());

/*
FIXME https://github.com/php/php-src/issues/20214
print "\nPDOStatement::setFetchMode:\n";
$stmt = $db->query($query);
$stmt->setFetchMode(\PDO::FETCH_DEFAULT);
print_r($stmt->fetch());
*/

print "\nPDOStatement::fetch:\n";
$stmt = $db->query($query);
print_r($stmt->fetch(\PDO::FETCH_DEFAULT));

print "\nPDOStatement::fetchAll:\n";
$stmt = $db->query($query);
print_r($stmt->fetchAll(\PDO::FETCH_DEFAULT));

print "\nPDO::setAttribute:\n";
try {
    $db->setAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE, \PDO::FETCH_DEFAULT);
} catch (ValueError $e) {
    print "Could not set fetch mode using PDO::setAttribute: ". $e->getMessage() ."\n";
}

if ($db->getAttribute(\PDO::ATTR_DEFAULT_FETCH_MODE) !== $expectedFetchMode) {
    print __LINE__ ." Fetch mode changed\n";
}

print "Done!\n";

--EXPECT--
Original:
stdClass Object
(
    [name] => Chris
    [country] => Ukraine
)

PDOStatement::fetch:
stdClass Object
(
    [name] => Chris
    [country] => Ukraine
)

PDOStatement::fetchAll:
Array
(
    [0] => stdClass Object
        (
            [name] => Chris
            [country] => Ukraine
        )

)

PDO::setAttribute:
Could not set fetch mode using PDO::setAttribute: PDO::setAttribute(): Argument #2 ($value) Fetch mode must be a bitmask of PDO::FETCH_* constants
Done!
