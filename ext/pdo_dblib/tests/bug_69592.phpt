--TEST--
PDO_DBLIB: PDO::DBLIB_ATTR_SKIP_EMPTY_ROWSETS for skip junk resultsets on SET NOCOUNT expression
--EXTENSIONS--
pdo_dblib
--SKIPIF--
<?php
require __DIR__ . '/config.inc';
getDbConnection();
?>
--FILE--
<?php
require __DIR__ . '/config.inc';

$db = getDbConnection();

$sql = '
    SET NOCOUNT ON
    SELECT 0 AS [result]
';

var_dump($db->getAttribute(PDO::DBLIB_ATTR_SKIP_EMPTY_ROWSETS));

$stmt = $db->query($sql);
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
var_dump($stmt->nextRowset());
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
$stmt->closeCursor();


$db->setAttribute(PDO::DBLIB_ATTR_SKIP_EMPTY_ROWSETS, true);
var_dump($db->getAttribute(PDO::DBLIB_ATTR_SKIP_EMPTY_ROWSETS));

$stmt = $db->query($sql);
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
var_dump($stmt->nextRowset());
var_dump($stmt->fetchAll(PDO::FETCH_ASSOC));
$stmt->closeCursor();
var_dump($db->getAttribute(PDO::DBLIB_ATTR_SKIP_EMPTY_ROWSETS));

?>
--EXPECT--
bool(false)
array(0) {
}
bool(true)
array(1) {
  [0]=>
  array(1) {
    ["result"]=>
    int(0)
  }
}
bool(true)
array(1) {
  [0]=>
  array(1) {
    ["result"]=>
    int(0)
  }
}
bool(false)
array(0) {
}
bool(true)
