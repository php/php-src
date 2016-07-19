--TEST--
PDO_DBLIB: Emulate how mssql extension names "computed" columns
--SKIPIF--
<?php
if (!extension_loaded('pdo_dblib')) die('skip not loaded');
require dirname(__FILE__) . '/config.inc';
?>
--FILE--
<?php
require dirname(__FILE__) . '/config.inc';

$stmt = $db->prepare("SELECT 1, 2 AS named, 3");
$stmt->execute();
var_dump($stmt->fetchAll());

?>
--EXPECT--
array(1) {
  [0]=>
  array(6) {
    ["computed"]=>
    int(1)
    [0]=>
    int(1)
    ["named"]=>
    int(2)
    [1]=>
    int(2)
    ["computed1"]=>
    int(3)
    [2]=>
    int(3)
  }
}
