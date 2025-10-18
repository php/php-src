--TEST--
PDO Common: PDO::FETCH_NAMED with PDO::FETCH_GROUP
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
$table = str_replace('.php', '', basename(__FILE__));
PDOTest::dropTableIfExists($db, $table);
?>
--FILE--
<?php
if (getenv('REDIR_TEST_DIR') === false) {
    putenv('REDIR_TEST_DIR=' . __DIR__ . '/../../pdo/tests/');
}
$table = str_replace('.php', '', basename(__FILE__));
require_once getenv('REDIR_TEST_DIR') . "/pdo_fetch_setup.php";

$query = "SELECT {$table}.country, {$table}.userid, {$table}.name, {$table}.country, {$table}.referred_by_userid, referrer.name
    FROM {$table}
    LEFT JOIN {$table} AS referrer ON {$table}.referred_by_userid = referrer.userid";
$fetchMode = \PDO::FETCH_NAMED | \PDO::FETCH_GROUP;

print "fetch:\n";
$stmt = $db->query($query . " WHERE {$table}.userid IN (104, 107)");
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
  ["country"]=>
  array(2) {
    [0]=>
    string(7) "Ukraine"
    [1]=>
    string(7) "Ukraine"
  }
  ["userid"]=>
  int(104)
  ["name"]=>
  array(2) {
    [0]=>
    string(5) "Chris"
    [1]=>
    NULL
  }
  ["referred_by_userid"]=>
  NULL
}
array(4) {
  ["country"]=>
  array(2) {
    [0]=>
    string(7) "Germany"
    [1]=>
    string(7) "Germany"
  }
  ["userid"]=>
  int(107)
  ["name"]=>
  array(2) {
    [0]=>
    string(5) "Robin"
    [1]=>
    string(5) "Chris"
  }
  ["referred_by_userid"]=>
  int(104)
}

fetchAll:
array(4) {
  ["Ukraine"]=>
  array(2) {
    [0]=>
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
    [1]=>
    array(4) {
      ["userid"]=>
      int(108)
      ["name"]=>
      array(2) {
        [0]=>
        string(4) "Sean"
        [1]=>
        NULL
      }
      ["country"]=>
      string(7) "Ukraine"
      ["referred_by_userid"]=>
      NULL
    }
  }
  ["England"]=>
  array(1) {
    [0]=>
    array(4) {
      ["userid"]=>
      int(105)
      ["name"]=>
      array(2) {
        [0]=>
        string(5) "Jamie"
        [1]=>
        NULL
      }
      ["country"]=>
      string(7) "England"
      ["referred_by_userid"]=>
      NULL
    }
  }
  ["Germany"]=>
  array(3) {
    [0]=>
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
    [1]=>
    array(4) {
      ["userid"]=>
      int(109)
      ["name"]=>
      array(2) {
        [0]=>
        string(4) "Toni"
        [1]=>
        NULL
      }
      ["country"]=>
      string(7) "Germany"
      ["referred_by_userid"]=>
      NULL
    }
    [2]=>
    array(4) {
      ["userid"]=>
      int(110)
      ["name"]=>
      array(2) {
        [0]=>
        string(4) "Toni"
        [1]=>
        NULL
      }
      ["country"]=>
      string(7) "Germany"
      ["referred_by_userid"]=>
      NULL
    }
  }
  ["France"]=>
  array(1) {
    [0]=>
    array(4) {
      ["userid"]=>
      int(111)
      ["name"]=>
      array(2) {
        [0]=>
        string(4) "Sean"
        [1]=>
        NULL
      }
      ["country"]=>
      string(6) "France"
      ["referred_by_userid"]=>
      NULL
    }
  }
}
