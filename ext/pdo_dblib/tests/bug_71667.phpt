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
    string(1) "1"
    [0]=>
    string(1) "1"
    ["named"]=>
    string(1) "2"
    [1]=>
    string(1) "2"
    ["computed1"]=>
    string(1) "3"
    [2]=>
    string(1) "3"
  }
}
