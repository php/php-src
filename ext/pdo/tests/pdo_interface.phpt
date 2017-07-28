--TEST--
Req #74957 - PDOInterface and PDOStatementInterface implemented
--SKIPIF--
<?php if (!extension_loaded('pdo')) die('skip'); ?>
--FILE--
<?php

$instance = new ReflectionClass('PDO');
$interfaces = $instance->getInterfaceNames();
var_dump($interfaces);

$interface = new ReflectionClass('PDOInterface');
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
array(1) {
  [0]=>
  string(%s) "PDOInterface"
}
prepare -> (statement, options, )
beginTransaction -> ()
commit -> ()
rollBack -> ()
inTransaction -> ()
setAttribute -> (attribute, value, )
exec -> (query, )
query -> ()
lastInsertId -> (seqname, )
errorCode -> ()
errorInfo -> ()
getAttribute -> (attribute, )
quote -> (string, paramtype, )

