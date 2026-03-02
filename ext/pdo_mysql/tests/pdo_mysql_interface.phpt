--TEST--
MySQL PDO class interface
--EXTENSIONS--
pdo_mysql
--SKIPIF--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
MySQLPDOTest::skip();
MySQLPDOTest::skipNotTransactionalEngine();
--FILE--
<?php
    require_once __DIR__ . '/inc/mysql_pdo_test.inc';
    $db = MySQLPDOTest::factory();

    $expected = [
        '__construct'						=> true,
        'connect' 							=> true,
        'prepare' 							=> true,
        'beginTransaction'					=> true,
        'commit'							=> true,
        'rollBack'							=> true,
        'setAttribute'						=> true,
        'exec'								=> true,
        'query'								=> true,
        'lastInsertId'						=> true,
        'errorCode'							=> true,
        'errorInfo'							=> true,
        'getAttribute'						=> true,
        'quote'								=> true,
        'inTransaction'						=> true,
        'getAvailableDrivers'				=> true,
    ];
    $classname = get_class($db);

    $methods = get_class_methods($classname);
    foreach ($methods as $k => $method) {
        if (isset($expected[$method])) {
            unset($expected[$method]);
            unset($methods[$k]);
        }
        if ($method == $classname) {
            unset($expected['__construct']);
            unset($methods[$k]);
        }
    }
    if (!empty($expected)) {
        printf("Dumping missing class methods\n");
        var_dump($expected);
    }
    if (!empty($methods)) {
        printf("Found more methods than expected, dumping list\n");
        var_dump($methods);
    }

    print "done!";
?>
--EXPECT--
done!
