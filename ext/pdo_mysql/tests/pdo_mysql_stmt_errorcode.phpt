--TEST--
MySQL PDOStatement->errorCode();
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

    $db->exec('DROP TABLE IF EXISTS pdo_mysql_stmt_errorcode_ihopeitdoesnotexist');

    printf("Testing emulated PS...\n");
    try {
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 1);
        if (1 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[002] Unable to turn on emulated prepared statements\n");

        $stmt = $db->prepare('SELECT id FROM pdo_mysql_stmt_errorcode_ihopeitdoesnotexist ORDER BY id ASC');
        $stmt->execute();
        var_dump($stmt->errorCode());


    } catch (PDOException $e) {
        printf("[001] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    printf("Testing native PS...\n");
    try {
        $db->setAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY, 0);
        if (0 != $db->getAttribute(PDO::MYSQL_ATTR_DIRECT_QUERY))
            printf("[004] Unable to turn off emulated prepared statements\n");

        $stmt = $db->prepare('SELECT id FROM pdo_mysql_stmt_errorcode_ihopeitdoesnotexist ORDER BY id ASC');
        $stmt->execute();
        var_dump($stmt->errorCode());

    } catch (PDOException $e) {
        printf("[003] %s [%s] %s\n",
            $e->getMessage(), $db->errorCode(), implode(' ', $db->errorInfo()));
    }

    print "done!";
?>
--EXPECTF--
Testing emulated PS...

Warning: PDOStatement::execute(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.pdo_mysql_stmt_errorcode_ihopeitdoesnotexist' doesn't exist in %s on line %d
string(5) "42S02"
Testing native PS...

Warning: PDO::prepare(): SQLSTATE[42S02]: Base table or view not found: 1146 Table '%s.pdo_mysql_stmt_errorcode_ihopeitdoesnotexist' doesn't exist in %s on line %d

Fatal error: Uncaught Error: Call to a member function execute() on false in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
