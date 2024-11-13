--TEST--
Bug #41125 (PDO mysql + quote() + prepare() can result in seg fault)
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

$search = "o'";
$sql = "SELECT 1 FROM DUAL WHERE 'o''riley' LIKE " . $db->quote('%' . $search . '%');
$stmt = $db->prepare($sql);
$stmt->execute();
print implode(' - ', (($r = @$stmt->fetch(PDO::FETCH_NUM)) ? $r : array())) ."\n";
print implode(' - ', $stmt->errorinfo()) ."\n";

print "-------------------------------------------------------\n";

$queries = array(
    "SELECT 1 FROM DUAL WHERE 1 = '?\'\''",
    "SELECT 'a\\'0' FROM DUAL WHERE 1 = ?",
    "SELECT 'a', 'b\'' FROM DUAL WHERE '''' LIKE '\\'' AND ?",
    "SELECT 'foo?bar', '', '''' FROM DUAL WHERE ?"
);

foreach ($queries as $k => $query) {
    $stmt = $db->prepare($query);
    $stmt->execute(array(1));
    printf("[%d] Query: [[%s]]\n", $k + 1, $query);
    print implode(' - ', (($r = @$stmt->fetch(PDO::FETCH_NUM)) ? $r : array())) ."\n";
    print implode(' - ', $stmt->errorinfo()) ."\n";
    print "--------\n";
}

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
$sql = "SELECT upper(:id) FROM DUAL WHERE '1'";
$stmt = $db->prepare($sql);

$id = 'o\'\0';
$stmt->bindParam(':id', $id);
$stmt->execute();
printf("Query: [[%s]]\n", $sql);
print implode(' - ', (($r = @$stmt->fetch(PDO::FETCH_NUM)) ? $r : array())) ."\n";
print implode(' - ', $stmt->errorinfo()) ."\n";

print "-------------------------------------------------------\n";

$queries = array(
    "SELECT 1, 'foo' FROM DUAL WHERE 1 = :id AND '\\0' IS NULL AND  2 <> :id",
    "SELECT 1 FROM DUAL WHERE 1 = :id AND '' AND  2 <> :id",
    "SELECT 1 FROM DUAL WHERE 1 = :id AND '\'\'' = '''' AND  2 <> :id",
    "SELECT 1 FROM DUAL WHERE 1 = :id AND '\'' = '''' AND  2 <> :id",
    "SELECT 'a', 'b\'' FROM DUAL WHERE '''' LIKE '\\'' AND 1",
    "SELECT 'a''', '\'b\'' FROM DUAL WHERE '''' LIKE '\\'' AND 1",
    "SELECT UPPER(:id) FROM DUAL WHERE '1'",
    "SELECT 1 FROM DUAL WHERE '\''",
    "SELECT 1 FROM DUAL WHERE :id AND '\\0' OR :id",
    "SELECT 1 FROM DUAL WHERE 'a\\f\\n\\0' AND 1 >= :id",
    "SELECT 1 FROM DUAL WHERE '\'' = ''''",
    "SELECT '\\n' '1 FROM DUAL WHERE '''' and :id'",
    "SELECT 1 'FROM DUAL WHERE :id AND '''' = '''' OR 1 = 1 AND ':id",
);

$db->setAttribute(PDO::ATTR_EMULATE_PREPARES, 1);
$id = 1;

foreach ($queries as $k => $query) {
    $stmt = $db->prepare($query);
    $stmt->bindParam(':id', $id);
    $stmt->execute();

    printf("[%d] Query: [[%s]]\n", $k + 1, $query);
    print implode(' - ', (($r = @$stmt->fetch(PDO::FETCH_NUM)) ? $r : array())) ."\n";
    print implode(' - ', $stmt->errorinfo()) ."\n";
    print "--------\n";
}

?>
--EXPECTF--
1
00000 -  - 
-------------------------------------------------------

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens in %s on line %d
[1] Query: [[SELECT 1 FROM DUAL WHERE 1 = '?\'\'']]

00000 -  - 
--------
[2] Query: [[SELECT 'a\'0' FROM DUAL WHERE 1 = ?]]
a'0
00000 -  - 
--------
[3] Query: [[SELECT 'a', 'b\'' FROM DUAL WHERE '''' LIKE '\'' AND ?]]
a - b'
00000 -  - 
--------
[4] Query: [[SELECT 'foo?bar', '', '''' FROM DUAL WHERE ?]]
foo?bar -  - '
00000 -  - 
--------
Query: [[SELECT upper(:id) FROM DUAL WHERE '1']]
O'\0
00000 -  - 
-------------------------------------------------------
[1] Query: [[SELECT 1, 'foo' FROM DUAL WHERE 1 = :id AND '\0' IS NULL AND  2 <> :id]]

00000 -  - 
--------
[2] Query: [[SELECT 1 FROM DUAL WHERE 1 = :id AND '' AND  2 <> :id]]

00000 -  - 
--------
[3] Query: [[SELECT 1 FROM DUAL WHERE 1 = :id AND '\'\'' = '''' AND  2 <> :id]]

00000 -  - 
--------
[4] Query: [[SELECT 1 FROM DUAL WHERE 1 = :id AND '\'' = '''' AND  2 <> :id]]
1
00000 -  - 
--------

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens in %s on line %d
[5] Query: [[SELECT 'a', 'b\'' FROM DUAL WHERE '''' LIKE '\'' AND 1]]

00000 -  - 
--------

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens in %s on line %d
[6] Query: [[SELECT 'a''', '\'b\'' FROM DUAL WHERE '''' LIKE '\'' AND 1]]

00000 -  - 
--------
[7] Query: [[SELECT UPPER(:id) FROM DUAL WHERE '1']]
1
00000 -  - 
--------

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens in %s on line %d
[8] Query: [[SELECT 1 FROM DUAL WHERE '\'']]

00000 -  - 
--------
[9] Query: [[SELECT 1 FROM DUAL WHERE :id AND '\0' OR :id]]
1
00000 -  - 
--------
[10] Query: [[SELECT 1 FROM DUAL WHERE 'a\f\n\0' AND 1 >= :id]]

00000 -  - 
--------

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens in %s on line %d
[11] Query: [[SELECT 1 FROM DUAL WHERE '\'' = '''']]

00000 -  - 
--------

Warning: PDOStatement::execute(): SQLSTATE[HY093]: Invalid parameter number: number of bound variables does not match number of tokens in %s on line %d
[12] Query: [[SELECT '\n' '1 FROM DUAL WHERE '''' and :id']]

00000 -  - 
--------
[13] Query: [[SELECT 1 'FROM DUAL WHERE :id AND '''' = '''' OR 1 = 1 AND ':id]]
1
00000 -  - 
--------
