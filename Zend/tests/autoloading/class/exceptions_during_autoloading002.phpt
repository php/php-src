--TEST--
Exceptions during autoloading
--FILE--
<?php

function MyAutoLoad($className)
{
    echo __METHOD__ . "($className)\n";
    throw new Exception('Bla');
}

class MyAutoLoader
{
    static function autoLoad($className)
    {
        echo __METHOD__ . "($className)\n";
        throw new Exception('Bla');
    }

    function dynaLoad($className)
    {
        echo __METHOD__ . "($className)\n";
        throw new Exception('Bla');
    }
}

$obj = new MyAutoLoader;

$funcs = array(
    'MyAutoLoad',
    'MyAutoLoader::autoLoad',
    'MyAutoLoader::dynaLoad',
    array('MyAutoLoader', 'autoLoad'),
    array('MyAutoLoader', 'dynaLoad'),
    array($obj, 'autoLoad'),
    array($obj, 'dynaLoad'),
);

foreach($funcs as $idx => $func)
{
    echo "====$idx====\n";

    var_dump($func);
    try {
        autoload_register_class($func);
    } catch (TypeError $e) {
        echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
        var_dump(count(autoload_list_class()));
        continue;
    }

    if (count(autoload_list_class())) {
        echo "registered\n";

        try {
            var_dump(class_exists("NoExistingTestClass", true));
        } catch (Exception $e) {
            echo get_class($e) . ': ' . $e->getMessage() . \PHP_EOL;
        }
    }

    autoload_unregister_class($func);
    var_dump(count(autoload_list_class()));
}

?>
--EXPECTF--
====0====
string(10) "MyAutoLoad"
registered
MyAutoLoad(NoExistingTestClass)
Exception: Bla
int(0)
====1====
string(22) "MyAutoLoader::autoLoad"
registered
MyAutoLoader::autoLoad(NoExistingTestClass)
Exception: Bla
int(0)
====2====
string(22) "MyAutoLoader::dynaLoad"
TypeError: autoload_register_class(): Argument #1 ($callback) must be a valid callback, non-static method MyAutoLoader::dynaLoad() cannot be called statically
int(0)
====3====
array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "autoLoad"
}
registered
MyAutoLoader::autoLoad(NoExistingTestClass)
Exception: Bla
int(0)
====4====
array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "dynaLoad"
}
TypeError: autoload_register_class(): Argument #1 ($callback) must be a valid callback, non-static method MyAutoLoader::dynaLoad() cannot be called statically
int(0)
====5====
array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "autoLoad"
}
registered
MyAutoLoader::autoLoad(NoExistingTestClass)
Exception: Bla
int(0)
====6====
array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "dynaLoad"
}
registered
MyAutoLoader::dynaLoad(NoExistingTestClass)
Exception: Bla
int(0)
