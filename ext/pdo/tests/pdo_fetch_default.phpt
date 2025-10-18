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
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR=' . __DIR__ . '/../../pdo/tests/');
}
require_once getenv('REDIR_TEST_DIR') . 'pdo_test.inc';
$db = PDOTest::factory();

print "Original:\n";
$stmt = $db->query("SELECT 'v1' AS c1, 'v2' AS c2");
print_r($stmt->fetch());

/*
FIXME https://github.com/php/php-src/issues/20214
print "\nPDOStatement::setFetchMode:\n";
$stmt = $db->query("SELECT 'v1' AS c1, 'v2' AS c2");
$stmt->setFetchMode(\PDO::FETCH_DEFAULT);
print_r($stmt->fetch());
*/

print "\nPDOStatement::fetch:\n";
$stmt = $db->query("SELECT 'v1' AS c1, 'v2' AS c2");
print_r($stmt->fetch(\PDO::FETCH_DEFAULT));

print "\nPDOStatement::fetchAll:\n";
$stmt = $db->query("SELECT 'v1' AS c1, 'v2' AS c2");
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
    [c1] => v1
    [c2] => v2
)

PDOStatement::fetch:
stdClass Object
(
    [c1] => v1
    [c2] => v2
)

PDOStatement::fetchAll:
Array
(
    [0] => stdClass Object
        (
            [c1] => v1
            [c2] => v2
        )

)

PDO::setAttribute:
Could not set fetch mode using PDO::setAttribute: Fetch mode must be a bitmask of PDO::FETCH_* constants
Done!
