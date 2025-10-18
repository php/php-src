--TEST--
PDO fetch mode NAMED with UNIQUE
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
require __DIR__ . "/../setup.php";

$query = "SELECT users.userid, users.name, users.country, users.referred_by_userid, referrer.name
    FROM users
    LEFT JOIN users AS referrer ON users.referred_by_userid = referrer.userid
    WHERE users.userid IN (104, 107)";
$fetchMode = \PDO::FETCH_NAMED | \PDO::FETCH_UNIQUE;

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
  int(104)
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
  int(107)
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
  int(104)
}

fetchAll:
array(2) {
  [104]=>
  array(3) {
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
  [107]=>
  array(3) {
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
    int(104)
  }
}
