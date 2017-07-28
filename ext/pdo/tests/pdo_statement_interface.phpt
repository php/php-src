--TEST--
Req #74957 - PDOInterface and PDOStatementInterface implemented
--SKIPIF--
<?php if (!extension_loaded('pdo')) die('skip'); ?>
--FILE--
<?php

$instance = new ReflectionClass('PDOStatement');
$interfaces = $instance->getInterfaceNames();
var_dump($interfaces);

$interface = new ReflectionClass('PDOStatementInterface');
$methods = $interface->getMethods();
foreach ($methods as $method) {
    echo $method->getName() . ' -> (';
    foreach($method->getParameters() as $param) {
        echo $param->getName() . ', ';
    }
    echo ")\n";
}
?>
--EXPECTF--
array(2) {
  [0]=>
  string(%s) "Traversable"
  [1]=>
  string(%s) "PDOStatementInterface"
}
execute -> (bound_input_params, )
fetch -> (how, orientation, offset, )
bindParam -> (paramno, param, type, maxlen, driverdata, )
bindColumn -> (column, param, type, maxlen, driverdata, )
bindValue -> (paramno, param, type, )
rowCount -> ()
fetchColumn -> (column_number, )
fetchAll -> (how, class_name, ctor_args, )
fetchObject -> (class_name, ctor_args, )
errorCode -> ()
errorInfo -> ()
setAttribute -> (attribute, value, )
getAttribute -> (attribute, )
columnCount -> ()
getColumnMeta -> (column, )
setFetchMode -> (mode, params, )
nextRowset -> ()
closeCursor -> ()
debugDumpParams -> ()
