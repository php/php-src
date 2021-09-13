--TEST--
JIT ASSIGN_DIM: 002
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--EXTENSIONS--
opcache
--FILE--
<?php
function foo() {
    $var[] = 1;
    var_dump($var);
}
foo();

function foo1() {
    $var1[] = true;
    var_dump($var1);
}
foo1();

function foo2() {
    $var2[] = array();
    var_dump($var2);
}
foo2();

function foo3() {
    $array = array(PHP_INT_MAX => "dummy");
    try {
        $array[] = array();
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }

    $array = new ArrayObject();
    $array[index()] = 1;
    $array[offset()] = 2;

    var_dump($array);
}
foo3();

function index() {
    return 2;
}

function offset() {
    return "a";
}

function foo4() {
    $array = array();
    $array[] = array();
    $array[0][] = 1;
    $array[0][1] = 1;
    var_dump($array);

    try {
        $array[function() {}] = 2;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    var_dump($array);

    $array2[][] = 3;
    var_dump($array);
}
foo4();

function foo5() {
    $a = 1;
    try {
        $a[2] = 1;
    } catch (Error $e) {
        echo $e->getMessage(), "\n";
    }
    return $a;
}
var_dump(foo5());

function false_to_array($a) {
    var_dump($a[2] = 1);
    return $a;
}
function false_to_array_append($a) {
    var_dump($a[] = 1);
    return $a;
}
function false_to_array_invalid_index($a) {
    var_dump($a[[]] = 1);
    return $a;
}
function false_to_array_nested($a) {
    var_dump($a[2][3] = 1);
    return $a;
}
function false_to_array_nested_append($a) {
    var_dump($a[][] = 1);
    return $a;
}
function false_to_array_nested_invalid_index($a) {
    var_dump($a[[]][0] = 1);
    return $a;
}
var_dump(false_to_array(false));
var_dump(false_to_array_append(false));
try {
    var_dump(false_to_array_invalid_index(false));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
var_dump(false_to_array_nested(false));
var_dump(false_to_array_nested_append(false));
try {
    var_dump(false_to_array_nested_invalid_index(false));
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

function array_access_undef() {
    $ao = new ArrayObject;
    $ao[0] = $undef;
    var_dump($ao[0]);
}

array_access_undef();
?>
--EXPECTF--
array(1) {
  [0]=>
  int(1)
}
array(1) {
  [0]=>
  bool(true)
}
array(1) {
  [0]=>
  array(0) {
  }
}
Cannot add element to the array as the next element is already occupied
object(ArrayObject)#%d (1) {
  ["storage":"ArrayObject":private]=>
  array(2) {
    [2]=>
    int(1)
    ["a"]=>
    int(2)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
}
Illegal offset type
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
}
array(1) {
  [0]=>
  array(2) {
    [0]=>
    int(1)
    [1]=>
    int(1)
  }
}
Cannot use a scalar value as an array
int(1)

Deprecated: Automatic conversion of false to array is deprecated in %s
int(1)
array(1) {
  [2]=>
  int(1)
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
int(1)
array(1) {
  [0]=>
  int(1)
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
Illegal offset type

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
int(1)
array(1) {
  [2]=>
  array(1) {
    [3]=>
    int(1)
  }
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
int(1)
array(1) {
  [0]=>
  array(1) {
    [0]=>
    int(1)
  }
}

Deprecated: Automatic conversion of false to array is deprecated in %s on line %d
Illegal offset type

Warning: Undefined variable $undef in %s on line %d
NULL
