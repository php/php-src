--TEST--
MySQL PDOStatement->nextRowSet() with PDO::MYSQL_ATTR_MULTI_STATEMENTS either true or false
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipVersionThanLess(50000);
?>
--FILE--
<?php
    require_once __DIR__ . '/inc/mysql_pdo_test.inc';
    $db = MySQLPDOTest::factory();
    $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);

    $table = 'pdo_mysql_multi_stmt_nextrowset';
    $procedure = 'pdo_mysql_multi_stmt_nextrowset_p';
    MySQLPDOTest::createTestTable($table, $db);

    function test_proc($db) {
        global $table;
        global $procedure;

        $db->exec("DROP PROCEDURE IF EXISTS {$procedure}");
        $db->exec("CREATE PROCEDURE {$procedure}() BEGIN SELECT id FROM $table ORDER BY id ASC LIMIT 3; SELECT id, label FROM {$table} WHERE id < 4 ORDER BY id DESC LIMIT 3; END;");
        $stmt = $db->query("CALL {$procedure}()");
        do {
            var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        } while ($stmt->nextRowSet());
        var_dump($stmt->nextRowSet());
    }

    try {
        // Using native PS for proc, since emulated fails.
        printf("Native PS...\n");
        foreach (array(false, true) as $multi) {
            $value = $multi ? 'true' : 'false';
            echo "\nTesting with PDO::MYSQL_ATTR_MULTI_STATEMENTS set to {$value}\n";
            $dsn = MySQLPDOTest::getDSN();
            $user = PDO_MYSQL_TEST_USER;
            $pass = PDO_MYSQL_TEST_PASS;
            $db = new PDO($dsn, $user, $pass, array(PDO::MYSQL_ATTR_MULTI_STATEMENTS => $multi));
            $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
            $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
            $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 1);
            $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
            test_proc($db);

            $db = new PDO($dsn, $user, $pass, array(PDO::MYSQL_ATTR_MULTI_STATEMENTS => $multi));
            $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
            $db->setAttribute(PDO::ATTR_STRINGIFY_FETCHES, true);
            $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, 0);
            $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);

            test_proc($db);

            // Switch back to emulated prepares to verify multi statement attribute.
            $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
            // This will fail when $multi is false.
            $stmt = $db->query("SELECT * FROM {$table}; INSERT INTO {$table} (id, label) VALUES (99, 'x')");
            if ($stmt !== false) {
                $stmt->closeCursor();
            }
            $info = $db->errorInfo();
            var_dump($info[0]);
        }
    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_multi_stmt_nextrowset');
$db->exec('DROP PROCEDURE IF EXISTS pdo_mysql_multi_stmt_nextrowset_p');
?>
--EXPECTF--
Native PS...

Testing with PDO::MYSQL_ATTR_MULTI_STATEMENTS set to false
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

Warning: PDO::query(): SQLSTATE[42000]: Syntax error or access violation: 1064 You have an error in your SQL syntax; check the manual that corresponds to your %s server version for the right syntax to use near 'INSERT INTO pdo_mysql_multi_stmt_nextrowset (id, label) VALUES (99, 'x')' at line 1 in %s on line %d
string(5) "42000"

Testing with PDO::MYSQL_ATTR_MULTI_STATEMENTS set to true
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
string(5) "00000"
done!
