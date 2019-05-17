--TEST--
Automatic promotion of falsy to object
--FILE--
<?php

class Test {
    public ?Test $prop;
    public ?stdClass $stdProp;
    public ?object $objectProp;

    public static ?Test $staticProp = null;
    public static ?stdClass $staticStdProp = null;
    public static ?object $staticObjectProp = null;
}

// Object properties
$test = new Test;
try {
    $test->prop->wat = 123;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$test->stdProp->wat = 123;
$test->objectProp->wat = 123;
var_dump($test);

// Object properties via reference
$test = new Test;
$prop =& $test->prop;
$stdProp =& $test->stdProp;
$objectProp =& $test->objectProp;
try {
    $prop->wat = 123;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$stdProp->wat = 123;
$objectProp->wat = 123;
var_dump($test);

// Object properties via reference rw
$test = new Test;
$prop =& $test->prop;
$stdProp =& $test->stdProp;
$objectProp =& $test->objectProp;
try {
    $prop->wat->wat = 123;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$stdProp->wat->wat = 123;
$objectProp->wat->wat = 123;
var_dump($test);

// Static properties
try {
    Test::$staticProp->wat = 123;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
Test::$staticStdProp->wat = 123;
Test::$staticObjectProp->wat = 123;
var_dump(Test::$staticProp, Test::$staticStdProp, Test::$staticObjectProp);

// Non-string property name
$test = new Test;
$propName = new class {
    public function __toString() {
        return 'prop';
    }
};
try {
    $test->$propName->wat = 123;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump($test);

// Initially null
$test = new Test;
$test->prop = NULL;
$test->stdProp = NULL;
$test->objectProp = NULL;
try {
    $test->prop->wat = 123;
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
$test->stdProp->wat = 123;
$test->objectProp->wat = 123;
var_dump($test);

?>
--EXPECTF--
Cannot auto-initialize an stdClass inside property Test::$prop of type ?Test

Warning: Creating default object from empty value in %s on line %d

Warning: Creating default object from empty value in %s on line %d
object(Test)#1 (2) {
  ["prop"]=>
  uninitialized(?Test)
  ["stdProp"]=>
  object(stdClass)#3 (1) {
    ["wat"]=>
    int(123)
  }
  ["objectProp"]=>
  object(stdClass)#4 (1) {
    ["wat"]=>
    int(123)
  }
}
Cannot auto-initialize an stdClass inside a reference held by property Test::$prop of type ?Test

Warning: Creating default object from empty value in %s on line %d

Warning: Creating default object from empty value in %s on line %d
object(Test)#5 (3) {
  ["prop"]=>
  &NULL
  ["stdProp"]=>
  &object(stdClass)#2 (1) {
    ["wat"]=>
    int(123)
  }
  ["objectProp"]=>
  &object(stdClass)#4 (1) {
    ["wat"]=>
    int(123)
  }
}
Cannot auto-initialize an stdClass inside a reference held by property Test::$prop of type ?Test

Warning: Creating default object from empty value in %s on line %d

Warning: Creating default object from empty value in %s on line %d

Warning: Creating default object from empty value in %s on line %d

Warning: Creating default object from empty value in %s on line %d
object(Test)#3 (3) {
  ["prop"]=>
  &NULL
  ["stdProp"]=>
  &object(stdClass)#1 (1) {
    ["wat"]=>
    object(stdClass)#2 (1) {
      ["wat"]=>
      int(123)
    }
  }
  ["objectProp"]=>
  &object(stdClass)#5 (1) {
    ["wat"]=>
    object(stdClass)#6 (1) {
      ["wat"]=>
      int(123)
    }
  }
}
Cannot auto-initialize an stdClass inside property Test::$staticProp of type ?Test

Warning: Creating default object from empty value in %s on line %d

Warning: Creating default object from empty value in %s on line %d
NULL
object(stdClass)#4 (1) {
  ["wat"]=>
  int(123)
}
object(stdClass)#8 (1) {
  ["wat"]=>
  int(123)
}
Cannot auto-initialize an stdClass inside property Test::$prop of type ?Test
object(Test)#9 (0) {
  ["prop"]=>
  uninitialized(?Test)
  ["stdProp"]=>
  uninitialized(?stdClass)
  ["objectProp"]=>
  uninitialized(?object)
}
Cannot auto-initialize an stdClass inside property Test::$prop of type ?Test

Warning: Creating default object from empty value in %s on line %d

Warning: Creating default object from empty value in %s on line %d
object(Test)#7 (3) {
  ["prop"]=>
  NULL
  ["stdProp"]=>
  object(stdClass)#10 (1) {
    ["wat"]=>
    int(123)
  }
  ["objectProp"]=>
  object(stdClass)#11 (1) {
    ["wat"]=>
    int(123)
  }
}
