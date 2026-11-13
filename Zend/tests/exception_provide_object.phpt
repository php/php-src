--TEST--
Exceptions providing object
--FILE--
<?php

class Test {
    public $random;

    function __construct() {
      $this->random = random_int(0, 100);
    }

    function doThrow(...$args) {
        throw new Exception();
    }

    function funcDoThrow() {
        return fn(...$args) => $this->doThrow(...$args);
    }
}

function doThrow(...$args) {
    (new Test())->funcDoThrow()(...$args);
}

echo "zend.exception_ignore_args=0\n";
echo "zend.exception_provide_object=1\n";
ini_set("zend.exception_ignore_args", 0);
ini_set("zend.exception_provide_object", 1);

try {
    doThrow("foo", "bar");
} catch (Exception $e) {
    var_dump($e->getTrace());
}

echo "zend.exception_ignore_args=1\n";
echo "zend.exception_provide_object=0\n";
ini_set("zend.exception_ignore_args", 1);
ini_set("zend.exception_provide_object", 0);

try {
    doThrow("foo", "bar");
} catch (Exception $e) {
    var_dump($e->getTrace());
}

?>
--EXPECTF--
zend.exception_ignore_args=0
zend.exception_provide_object=1
array(3) {
  [0]=>
  array(7) {
    ["file"]=>
    string(%d) "%sexception_provide_object.php"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(7) "doThrow"
    ["class"]=>
    string(4) "Test"
    ["object"]=>
    object(Test)#%d (%d) {
      ["random"]=>
      int(%d)
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(2) {
      [0]=>
      string(3) "foo"
      [1]=>
      string(3) "bar"
    }
  }
  [1]=>
  array(7) {
    ["file"]=>
    string(%d) "%sexception_provide_object.php"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(%d) "{closure:Test::funcDoThrow():%d}"
    ["class"]=>
    string(4) "Test"
    ["object"]=>
    object(Test)#%d (%d) {
      ["random"]=>
      int(%d)
    }
    ["type"]=>
    string(2) "->"
    ["args"]=>
    array(2) {
      [0]=>
      string(3) "foo"
      [1]=>
      string(3) "bar"
    }
  }
  [2]=>
  array(4) {
    ["file"]=>
    string(%d) "%sexception_provide_object.php"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(7) "doThrow"
    ["args"]=>
    array(2) {
      [0]=>
      string(3) "foo"
      [1]=>
      string(3) "bar"
    }
  }
}
zend.exception_ignore_args=1
zend.exception_provide_object=0
array(3) {
  [0]=>
  array(5) {
    ["file"]=>
    string(%d) "%sexception_provide_object.php"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(7) "doThrow"
    ["class"]=>
    string(4) "Test"
    ["type"]=>
    string(2) "->"
  }
  [1]=>
  array(5) {
    ["file"]=>
    string(%d) "%sexception_provide_object.php"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(%d) "{closure:Test::funcDoThrow():%d}"
    ["class"]=>
    string(4) "Test"
    ["type"]=>
    string(2) "->"
  }
  [2]=>
  array(3) {
    ["file"]=>
    string(%d) "%sexception_provide_object.php"
    ["line"]=>
    int(%d)
    ["function"]=>
    string(7) "doThrow"
  }
}
