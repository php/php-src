--TEST--
MySQL PDOStatement->nextRowSet()
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
MySQLPDOTest::skip();
$db = MySQLPDOTest::factory();
$row = $db->query('SELECT VERSION() as _version')->fetch(PDO::FETCH_ASSOC);
$matches = array();
if (!preg_match('/^(\d+)\.(\d+)\.(\d+)/ismU', $row['_version'], $matches))
    die(sprintf("skip Cannot determine MySQL Server version\n"));

$version = $matches[1] * 10000 + $matches[2] * 100 + $matches[3];
if ($version < 50000)
    die(sprintf("skip Need MySQL Server 5.0.0+, found %d.%02d.%02d (%d)\n",
        $matches[1], $matches[2], $matches[3], $version));

if (!MySQLPDOTest::isPDOMySQLnd())
    die("skip This will not work with libmysql");
?>
--FILE--
<?php
    require_once(__DIR__ . DIRECTORY_SEPARATOR . 'mysql_pdo_test.inc');
    $db = MySQLPDOTest::factory();
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

    MySQLPDOTest::createTestTable($db);

    $stmt = $db->query('SELECT id FROM test');
    if (false !== ($tmp = $stmt->nextRowSet()))
        printf("[002] Expecting false got %s\n", var_export($tmp, true));

    function test_proc1($db) {

        $stmt = $db->query('SELECT @VERSION as _version');
        $tmp = $stmt->fetch(PDO::FETCH_ASSOC);
        assert($tmp['_version'] === NULL);
        while ($stmt->fetch()) ;

        $db->exec('DROP PROCEDURE IF EXISTS p');
        $db->exec('CREATE PROCEDURE p(OUT ver_param VARCHAR(25)) BEGIN SELECT VERSION() INTO ver_param; END;');
        $db->exec('CALL p(@VERSION)');
        $stmt = $db->query('SELECT @VERSION as _version');
        var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        var_dump($stmt->nextRowSet());

    }

    function test_proc2($db) {

        $db->exec('DROP PROCEDURE IF EXISTS p');
        $db->exec('CREATE PROCEDURE p() BEGIN SELECT id FROM test ORDER BY id ASC LIMIT 3; SELECT id, label FROM test WHERE id < 4 ORDER BY id DESC LIMIT 3; END;');
        $stmt = $db->query('CALL p()');
        do {
            var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        } while ($stmt->nextRowSet());
        var_dump($stmt->nextRowSet());

        echo "Skip fetchAll(): ";
        unset($stmt);
        $stmt = $db->query('CALL p()');
        var_dump($stmt->nextRowSet());
        $stmt->closeCursor();
    }

    try {

        // Emulated PS
        printf("Emulated PS...\n");
        $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);

        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 1);
        test_proc1($db);
        test_proc2($db);

        $db = MySQLPDOTest::factory();
        $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
        $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 0);
        test_proc1($db);
        test_proc2($db);

        // Native PS
        printf("Native PS...\n");
        $db = MySQLPDOTest::factory();
        $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 1);
        $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
        test_proc1($db);
        test_proc2($db);

        $db = MySQLPDOTest::factory();
        $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
        $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 0);
        $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);

        test_proc1($db);
        test_proc2($db);

        @$db->exec('DROP PROCEDURE IF EXISTS p');

    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--CLEAN--
<?php
require __DIR__ . '/mysql_pdo_test.inc';
MySQLPDOTest::dropTestTable();
?>
--EXPECTF--
Emulated PS...
array(1) {
  [0]=>
  array(1) {
    ["_version"]=>
    string(%d) "%s"
  }
}
bool(false)
array(3) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    ["id"]=>
    string(1) "2"
  }
  [2]=>
  array(1) {
    ["id"]=>
    string(1) "3"
  }
}
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "3"
    ["label"]=>
    string(1) "c"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
  }
  [2]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
  }
}
array(0) {
}
bool(false)
Skip fetchAll(): bool(true)
array(1) {
  [0]=>
  array(1) {
    ["_version"]=>
    string(%d) "%s"
  }
}
bool(false)
array(3) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    ["id"]=>
    string(1) "2"
  }
  [2]=>
  array(1) {
    ["id"]=>
    string(1) "3"
  }
}
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "3"
    ["label"]=>
    string(1) "c"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
  }
  [2]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
  }
}
array(0) {
}
bool(false)
Skip fetchAll(): bool(true)
Native PS...
array(1) {
  [0]=>
  array(1) {
    ["_version"]=>
    string(%d) "%s"
  }
}
bool(false)
array(3) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    ["id"]=>
    string(1) "2"
  }
  [2]=>
  array(1) {
    ["id"]=>
    string(1) "3"
  }
}
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "3"
    ["label"]=>
    string(1) "c"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
  }
  [2]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
  }
}
array(0) {
}
bool(false)
Skip fetchAll(): bool(true)
array(1) {
  [0]=>
  array(1) {
    ["_version"]=>
    string(%d) "%s"
  }
}
bool(false)
array(3) {
  [0]=>
  array(1) {
    ["id"]=>
    string(1) "1"
  }
  [1]=>
  array(1) {
    ["id"]=>
    string(1) "2"
  }
  [2]=>
  array(1) {
    ["id"]=>
    string(1) "3"
  }
}
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    string(1) "3"
    ["label"]=>
    string(1) "c"
  }
  [1]=>
  array(2) {
    ["id"]=>
    string(1) "2"
    ["label"]=>
    string(1) "b"
  }
  [2]=>
  array(2) {
    ["id"]=>
    string(1) "1"
    ["label"]=>
    string(1) "a"
  }
}
array(0) {
}
bool(false)
Skip fetchAll(): bool(true)
done!
