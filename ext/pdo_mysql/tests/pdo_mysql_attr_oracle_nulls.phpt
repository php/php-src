--TEST--
PDO::ATTR_ORACLE_NULLS
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

    $procedure = 'pdo_mysql_attr_oracle_nulls_p';

    try {
        $db->setAttribute(PDO::ATTR_ORACLE_NULLS, []);
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }
    try {
        $db->setAttribute(PDO::ATTR_ORACLE_NULLS, new stdClass());
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }
    try {
        $db->setAttribute(PDO::ATTR_ORACLE_NULLS, 'pdo');
    } catch (\TypeError $e) {
        echo $e->getMessage(), \PHP_EOL;
    }

    $db->setAttribute(PDO::ATTR_ORACLE_NULLS, 1);
    $stmt = $db->query("SELECT NULL AS z, '' AS a, ' ' AS b, TRIM(' ') as c, ' d' AS d, '" . chr(0) . " e' AS e");
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    $db->setAttribute(PDO::ATTR_ORACLE_NULLS, 0);
    $stmt = $db->query("SELECT NULL AS z, '' AS a, ' ' AS b, TRIM(' ') as c, ' d' AS d, '" . chr(0) . " e' AS e");
    var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));

    $db->setAttribute(PDO::ATTR_ORACLE_NULLS, 1);
    $stmt = $db->query('SELECT VERSION() as _version');
    $row = $stmt->fetch(PDO::FETCH_ASSOC);
    if ((int)strtok($row['_version'], '.') >= 5)
        $have_procedures = true;
    else
        $have_procedures = false;

    $db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 0);
    $db->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY, false);

    if ($have_procedures && (false !== $db->exec("DROP PROCEDURE IF EXISTS {$procedure}")) &&
        (false !== $db->exec("CREATE PROCEDURE {$procedure}() BEGIN SELECT NULL as z, '' AS a, ' ' AS b, TRIM(' ') as c, ' d' AS d, ' e' AS e; END;"))) {
        // requires MySQL 5+
        $stmt = $db->prepare("CALL {$procedure}()");
        $stmt->execute();
        do {
            var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        } while ($stmt->nextRowset());

        $stmt->execute();
        do {
            var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
        } while ($stmt->nextRowset());

    }

    print "done!";
?>
--CLEAN--
<?php
require_once __DIR__ . '/inc/mysql_pdo_test.inc';
$db = MySQLPDOTest::factory();
$db->exec("DROP PROCEDURE IF EXISTS pdo_mysql_attr_oracle_nulls_p");
?>
--EXPECTF--
Attribute value must be of type int for selected attribute, array given
Attribute value must be of type int for selected attribute, stdClass given
Attribute value must be of type int for selected attribute, string given
array(1) {
  [0]=>
  array(6) {
    ["z"]=>
    NULL
    ["a"]=>
    NULL
    ["b"]=>
    string(1) " "
    ["c"]=>
    NULL
    ["d"]=>
    string(2) " d"
    ["e"]=>
    string(3) "%se"
  }
}
array(1) {
  [0]=>
  array(6) {
    ["z"]=>
    NULL
    ["a"]=>
    string(0) ""
    ["b"]=>
    string(1) " "
    ["c"]=>
    string(0) ""
    ["d"]=>
    string(2) " d"
    ["e"]=>
    string(3) "%se"
  }
}
array(1) {
  [0]=>
  array(6) {
    ["z"]=>
    NULL
    ["a"]=>
    NULL
    ["b"]=>
    string(1) " "
    ["c"]=>
    NULL
    ["d"]=>
    string(2) " d"
    ["e"]=>
    string(2) " e"
  }
}
array(0) {
}
array(1) {
  [0]=>
  array(6) {
    ["z"]=>
    NULL
    ["a"]=>
    NULL
    ["b"]=>
    string(1) " "
    ["c"]=>
    NULL
    ["d"]=>
    string(2) " d"
    ["e"]=>
    string(2) " e"
  }
}
array(0) {
}
done!
