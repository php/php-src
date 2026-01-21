--TEST--
SPL: spl_autoload() with inaccessible methods
--INI--
include_path=.
--FILE--
<?php

class MyAutoLoader {

        static protected function noAccess($className) {
            echo __METHOD__ . "($className)\n";
        }

        static function autoLoad($className) {
            echo __METHOD__ . "($className)\n";
        }

        function dynaLoad($className) {
            echo __METHOD__ . "($className)\n";
        }
}

$obj = new MyAutoLoader;

$funcs = array(
    'MyAutoLoader::notExist',
    'MyAutoLoader::noAccess',
    'MyAutoLoader::autoLoad',
    'MyAutoLoader::dynaLoad',
    array('MyAutoLoader', 'notExist'),
    array('MyAutoLoader', 'noAccess'),
    array('MyAutoLoader', 'autoLoad'),
    array('MyAutoLoader', 'dynaLoad'),
    array($obj, 'notExist'),
    array($obj, 'noAccess'),
    array($obj, 'autoLoad'),
    array($obj, 'dynaLoad'),
);

foreach($funcs as $idx => $func)
{
    if ($idx) echo "\n";
    try {
        var_dump($func);
        spl_autoload_register($func);
        echo "ok\n";
    } catch(\TypeError $e) {
        echo $e->getMessage() . \PHP_EOL;
    }
}

?>
--EXPECTF--
string(22) "MyAutoLoader::notExist"
spl_autoload_register(): Argument #1 ($callback) must be a valid callback or null, class MyAutoLoader does not have a method "notExist"

string(22) "MyAutoLoader::noAccess"
spl_autoload_register(): Argument #1 ($callback) must be a valid callback or null, cannot access protected method MyAutoLoader::noAccess()

string(22) "MyAutoLoader::autoLoad"
ok

string(22) "MyAutoLoader::dynaLoad"
spl_autoload_register(): Argument #1 ($callback) must be a valid callback or null, non-static method MyAutoLoader::dynaLoad() cannot be called statically

array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "notExist"
}
spl_autoload_register(): Argument #1 ($callback) must be a valid callback or null, class MyAutoLoader does not have a method "notExist"

array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "noAccess"
}
spl_autoload_register(): Argument #1 ($callback) must be a valid callback or null, cannot access protected method MyAutoLoader::noAccess()

array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "autoLoad"
}
ok

array(2) {
  [0]=>
  string(12) "MyAutoLoader"
  [1]=>
  string(8) "dynaLoad"
}
spl_autoload_register(): Argument #1 ($callback) must be a valid callback or null, non-static method MyAutoLoader::dynaLoad() cannot be called statically

array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "notExist"
}
spl_autoload_register(): Argument #1 ($callback) must be a valid callback or null, class MyAutoLoader does not have a method "notExist"

array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "noAccess"
}
spl_autoload_register(): Argument #1 ($callback) must be a valid callback or null, cannot access protected method MyAutoLoader::noAccess()

array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "autoLoad"
}
ok

array(2) {
  [0]=>
  object(MyAutoLoader)#%d (0) {
  }
  [1]=>
  string(8) "dynaLoad"
}
ok
