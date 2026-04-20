--TEST--
PDO Common: PDO::FETCH_NAMED
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
PDOTest::dropTableIfExists($db, 'pdo_fetch_named');
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR=' . __DIR__ . '/../../pdo/tests/');
}
$table = 'pdo_fetch_named';
require_once getenv('REDIR_TEST_DIR') . "/pdo_fetch_setup.php";

$query = "SELECT {$table}.userid, {$table}.name, {$table}.country, {$table}.referred_by_userid, referrer.name
    FROM {$table}
    LEFT JOIN {$table} AS referrer ON {$table}.referred_by_userid = referrer.userid
    WHERE {$table}.userid IN (104, 107)";
$fetchMode = \PDO::FETCH_NAMED;

print "fetch:\n";
$stmt = $db->query($query);
while (false !== ($result = $stmt->fetch($fetchMode))) {
    var_dump($result);
}

print "\nfetchAll:\n";
$stmt = $db->query($query);
$result = $stmt->fetchAll($fetchMode);
var_dump($result);

--EXPECT--
fetch:
array(4) {
  ["userid"]=>
  string(3) "104"
  ["name"]=>
  array(2) {
    [0]=>
    string(5) "Chris"
    [1]=>
    NULL
  }
  ["country"]=>
  string(7) "Ukraine"
  ["referred_by_userid"]=>
  NULL
}
array(4) {
  ["userid"]=>
  string(3) "107"
  ["name"]=>
  array(2) {
    [0]=>
    string(5) "Robin"
    [1]=>
    string(5) "Chris"
  }
  ["country"]=>
  string(7) "Germany"
  ["referred_by_userid"]=>
  string(3) "104"
}

fetchAll:
array(2) {
  [0]=>
  array(4) {
    ["userid"]=>
    string(3) "104"
    ["name"]=>
    array(2) {
      [0]=>
      string(5) "Chris"
      [1]=>
      NULL
    }
    ["country"]=>
    string(7) "Ukraine"
    ["referred_by_userid"]=>
    NULL
  }
  [1]=>
  array(4) {
    ["userid"]=>
    string(3) "107"
    ["name"]=>
    array(2) {
      [0]=>
      string(5) "Robin"
      [1]=>
      string(5) "Chris"
    }
    ["country"]=>
    string(7) "Germany"
    ["referred_by_userid"]=>
    string(3) "104"
  }
}
