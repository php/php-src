--TEST--
MySQL PDOStatement->errorInfo();
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
?>
--FILE--
<?php
    require_once __DIR__ . '/inc/mysql_pdo_test.inc';
    $db = MySQLPDOTest::factory();

    $table = 'pdo_mysql_stmt_errorinfo';
    MySQLPDOTest::createTestTable($table , $db);

    printf("Testing emulated PS...\n");
    try {
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
        if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[002] Unable to turn on emulated prepared statements\n");

        $stmt = $db->prepare('SELECT id FROM pdo_mysql_stmt_errorinfo_ihopeitdoesnotexist ORDER BY id ASC');
        var_dump($stmt->errorInfo());
        $stmt->execute();
        var_dump($stmt->errorInfo());

        MySQLPDOTest::createTestTable($table, $db);
        $stmt = $db->prepare("SELECT label FROM {$table} ORDER BY id ASC LIMIT 1");
        $db->exec("DROP TABLE {$table}");
        var_dump($stmt->execute());
        var_dump($stmt->errorInfo());
        var_dump($db->errorInfo());

    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorInfo(), implode(' ', $db->errorInfo()));
    }

    printf("Testing native PS...\n");
    try {
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
        if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[004] Unable to turn off emulated prepared statements\n");

        $stmt = $db->prepare('SELECT id FROM pdo_mysql_stmt_errorinfo_ihopeitdoesnotexist ORDER BY id ASC');
        var_dump($stmt);

        MySQLPDOTest::createTestTable($table, $db);
        $stmt = $db->prepare("SELECT label FROM {$table} ORDER BY id ASC LIMIT 1");
        var_dump($stmt->errorInfo());
        $db->exec("DROP TABLE {$table}");
        $stmt->execute();
        var_dump($stmt->errorInfo());
        var_dump($db->errorInfo());

    } catch (PDOException $e) {
        printf("[003] %s [%s] %s\n",
            $e->getMessage(), $db->errorInfo(), implode(' ', $db->errorInfo()));
    }
    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec('DROP TABLE IF EXISTS pdo_mysql_stmt_errorinfo');
?>
--EXPECTF--
Testing emulated PS...
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  NULL
  [2]=>
  NULL
}

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.pdo_mysql_stmt_errorinfo_ihopeitdoesnotexist' doesn't exist in %s on line %d
array(3) {
  [0]=>
  string(5) "42S02"
  [1]=>
  int(1146)
  [2]=>
  string(%d) "Table '%s.pdo_mysql_stmt_errorinfo_ihopeitdoesnotexist' doesn't exist"
}

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.pdo_mysql_stmt_errorinfo' doesn't exist in %s on line %d
bool(false)
array(3) {
  [0]=>
  string(5) "42S02"
  [1]=>
  int(1146)
  [2]=>
  string(%d) "Table '%s.pdo_mysql_stmt_errorinfo' doesn't exist"
}
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
Testing native PS...

Warning: PDO::prepare(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.pdo_mysql_stmt_errorinfo_ihopeitdoesnotexist' doesn't exist in %s on line %d
bool(false)
array(3) {
  [0]=>
  string(0) ""
  [1]=>
  NULL
  [2]=>
  NULL
}

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.pdo_mysql_stmt_errorinfo' doesn't exist in %s on line %d
array(3) {
  [0]=>
  string(5) "42S02"
  [1]=>
  int(1146)
  [2]=>
  string(%d) "Table '%s.pdo_mysql_stmt_errorinfo' doesn't exist"
}
array(3) {
  [0]=>
  string(5) "00000"
  [1]=>
  NULL
  [2]=>
  NULL
}
done!
