--TEST--
Test get_class_vars() function : testing visibility
--FILE--
<?php
class Ancestor {
  function test() {
    var_dump(get_class_vars("Tester"));
  }

  static function testStatic() {
    var_dump(get_class_vars("Tester"));
  }
}

class Tester extends Ancestor {
  public $pub = "public var";
  protected $prot = "protected var";
  private $priv = "private var";

  static public $pubs = "public static var";
  static protected $prots = "protected static var";
  static private $privs = "private static var";

  function test() {
    var_dump(get_class_vars("Tester"));
  }

  static function testStatic() {
    var_dump(get_class_vars("Tester"));
  }
}

class Child extends Tester {
  function test() {
    var_dump(get_class_vars("Tester"));
  }

  static function testStatic() {
    var_dump(get_class_vars("Tester"));
  }
}

echo "*** Testing get_class_vars() : testing visibility\n";

echo "\n-- From global context --\n";
var_dump(get_class_vars("Tester"));

echo "\n-- From inside an object instance --\n";
$instance = new Tester();
$instance->test();

echo "\n-- From  a static context --\n";
Tester::testStatic();


echo "\n-- From inside an  parent object instance --\n";
$parent = new Ancestor();
$parent->test();

echo "\n-- From a parents static context --\n";
Ancestor::testStatic();


echo "\n-- From inside a child object instance --\n";
$child = new Child();
$child->test();

echo "\n-- From a child's static context --\n";
Child::testStatic();
?>
--EXPECT--
*** Testing get_class_vars() : testing visibility

-- From global context --
array(2) {
  ["pub"]=>
  string(10) "public var"
  ["pubs"]=>
  string(17) "public static var"
}

-- From inside an object instance --
array(6) {
  ["pub"]=>
  string(10) "public var"
  ["prot"]=>
  string(13) "protected var"
  ["priv"]=>
  string(11) "private var"
  ["pubs"]=>
  string(17) "public static var"
  ["prots"]=>
  string(20) "protected static var"
  ["privs"]=>
  string(18) "private static var"
}

-- From  a static context --
array(6) {
  ["pub"]=>
  string(10) "public var"
  ["prot"]=>
  string(13) "protected var"
  ["priv"]=>
  string(11) "private var"
  ["pubs"]=>
  string(17) "public static var"
  ["prots"]=>
  string(20) "protected static var"
  ["privs"]=>
  string(18) "private static var"
}

-- From inside an  parent object instance --
array(4) {
  ["pub"]=>
  string(10) "public var"
  ["prot"]=>
  string(13) "protected var"
  ["pubs"]=>
  string(17) "public static var"
  ["prots"]=>
  string(20) "protected static var"
}

-- From a parents static context --
array(4) {
  ["pub"]=>
  string(10) "public var"
  ["prot"]=>
  string(13) "protected var"
  ["pubs"]=>
  string(17) "public static var"
  ["prots"]=>
  string(20) "protected static var"
}

-- From inside a child object instance --
array(4) {
  ["pub"]=>
  string(10) "public var"
  ["prot"]=>
  string(13) "protected var"
  ["pubs"]=>
  string(17) "public static var"
  ["prots"]=>
  string(20) "protected static var"
}

-- From a child's static context --
array(4) {
  ["pub"]=>
  string(10) "public var"
  ["prot"]=>
  string(13) "protected var"
  ["pubs"]=>
  string(17) "public static var"
  ["prots"]=>
  string(20) "protected static var"
}
