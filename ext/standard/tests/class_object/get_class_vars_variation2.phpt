--TEST--
Test get_class_vars() function : testing visibility 
--FILE--
<?php
/* Prototype  : array get_class_vars(string class_name)
 * Description: Returns an array of default properties of the class. 
 * Source code: Zend/zend_builtin_functions.c
 * Alias to functions: 
 */

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
===DONE===
--EXPECT--
*** Testing get_class_vars() : testing visibility

-- From global context --
array(2) {
  [u"pub"]=>
  unicode(10) "public var"
  [u"pubs"]=>
  unicode(17) "public static var"
}

-- From inside an object instance --
array(6) {
  [u"pub"]=>
  unicode(10) "public var"
  [u"prot"]=>
  unicode(13) "protected var"
  [u"priv"]=>
  unicode(11) "private var"
  [u"pubs"]=>
  unicode(17) "public static var"
  [u"prots"]=>
  unicode(20) "protected static var"
  [u"privs"]=>
  unicode(18) "private static var"
}

-- From  a static context --
array(6) {
  [u"pub"]=>
  unicode(10) "public var"
  [u"prot"]=>
  unicode(13) "protected var"
  [u"priv"]=>
  unicode(11) "private var"
  [u"pubs"]=>
  unicode(17) "public static var"
  [u"prots"]=>
  unicode(20) "protected static var"
  [u"privs"]=>
  unicode(18) "private static var"
}

-- From inside an  parent object instance --
array(4) {
  [u"pub"]=>
  unicode(10) "public var"
  [u"prot"]=>
  unicode(13) "protected var"
  [u"pubs"]=>
  unicode(17) "public static var"
  [u"prots"]=>
  unicode(20) "protected static var"
}

-- From a parents static context --
array(4) {
  [u"pub"]=>
  unicode(10) "public var"
  [u"prot"]=>
  unicode(13) "protected var"
  [u"pubs"]=>
  unicode(17) "public static var"
  [u"prots"]=>
  unicode(20) "protected static var"
}

-- From inside a child object instance --
array(4) {
  [u"pub"]=>
  unicode(10) "public var"
  [u"prot"]=>
  unicode(13) "protected var"
  [u"pubs"]=>
  unicode(17) "public static var"
  [u"prots"]=>
  unicode(20) "protected static var"
}

-- From a child's static context --
array(4) {
  [u"pub"]=>
  unicode(10) "public var"
  [u"prot"]=>
  unicode(13) "protected var"
  [u"pubs"]=>
  unicode(17) "public static var"
  [u"prots"]=>
  unicode(20) "protected static var"
}
===DONE===