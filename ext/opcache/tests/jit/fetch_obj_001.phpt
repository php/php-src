--TEST--
JIT: FETCH_OBJ
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--EXTENSIONS--
opcache
--FILE--
<?php
function foo(&$a) {
    $a = 2;
}

function foo2(&$a) {
    $a = array();
}

function foo3(&$a, $var) {
    $a = $var;
}

$obj = new stdClass;
foo($obj->a);
var_dump($obj);
foo2($obj->b);
var_dump($obj);
foo3($obj->a, "2" . "3");
foo3($obj->a, $obj->b);
var_dump($obj);

$a = &$obj->a;
$a = fopen(__FILE__, "r");
var_dump($obj);

function bar() {
    $obj = new stdClass;
    foo($obj->a);
    var_dump($obj);
    foo2($obj->b);
    var_dump($obj);
    foo3($obj->a, "2" . "3");
    foo3($obj->a, $obj->b);
    var_dump($obj);

    $a = &$obj->a;
    $a = fopen(__FILE__, "r");
    var_dump($obj);

    $d = array();
    try {
        foo($d->{"ab" ."c"});
    } catch (Error $err) {
        echo $err->getMessage(), "\n";
    }
    var_dump($d);

    $e = NULL;
    try {
        foo($e->{"ab" ."c"});
    } catch (Error $err) {
        echo $err->getMessage(), "\n";
    }
    var_dump($e);

    $f = "";
    try {
        foo($f->{"ab" ."c"});
    } catch (Error $err) {
        echo $err->getMessage(), "\n";
    }
    var_dump($f);
}

bar();
?>
--EXPECTF--
object(stdClass)#%d (1) {
  ["a"]=>
  int(2)
}
object(stdClass)#%d (2) {
  ["a"]=>
  int(2)
  ["b"]=>
  array(0) {
  }
}
object(stdClass)#%d (2) {
  ["a"]=>
  array(0) {
  }
  ["b"]=>
  array(0) {
  }
}
object(stdClass)#%d (2) {
  ["a"]=>
  &resource(5) of type (stream)
  ["b"]=>
  array(0) {
  }
}
object(stdClass)#%d (1) {
  ["a"]=>
  int(2)
}
object(stdClass)#%d (2) {
  ["a"]=>
  int(2)
  ["b"]=>
  array(0) {
  }
}
object(stdClass)#%d (2) {
  ["a"]=>
  array(0) {
  }
  ["b"]=>
  array(0) {
  }
}
object(stdClass)#%d (2) {
  ["a"]=>
  &resource(6) of type (stream)
  ["b"]=>
  array(0) {
  }
}
Attempt to modify property "abc" on array
array(0) {
}
Attempt to modify property "abc" on null
NULL
Attempt to modify property "abc" on string
string(0) ""
