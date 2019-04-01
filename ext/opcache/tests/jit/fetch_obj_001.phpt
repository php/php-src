--TEST--
JIT: FETCH_OBJ
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.file_update_protection=0
opcache.jit_buffer_size=1M
--SKIPIF--
<?php require_once('../skipif.inc'); ?>
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
    foo($d->{"ab" ."c"});
	var_dump($d);

    $e = NULL;
    foo($e->{"ab" ."c"});
	var_dump($e);

    $f = "";
    foo($f->{"ab" ."c"});
	var_dump($f);
}

bar();
?>
--EXPECTF--
Warning: Creating default object from empty value in %s on line 14
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

Warning: Creating default object from empty value in %s on line 27
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

Warning: Attempt to modify property 'abc' of non-object in %sfetch_obj_001.php on line 40
array(0) {
}

Warning: Creating default object from empty value in %s on line 44
object(stdClass)#%d (1) {
  ["abc"]=>
  int(2)
}

Warning: Creating default object from empty value in %s on line 48
object(stdClass)#%d (1) {
  ["abc"]=>
  int(2)
}
