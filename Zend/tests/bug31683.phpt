--TEST--
Bug #31683 (changes to $name in __get($name) override future parameters)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php

class Foo implements ArrayAccess {

  function __get($test) {
    var_dump($test);         
    $test = 'bug';
  }

  function __set($test, $val) {
    var_dump($test);         
    var_dump($val);
    $test = 'bug';
    $val = 'bug';
  }

  function __call($test, $arg) {
    var_dump($test);         
    $test = 'bug';
  }

  function offsetget($test) {
    var_dump($test);         
    $test = 'bug';
    return 123;
  }

  function offsetset($test, $val) {
    var_dump($test);         
    var_dump($val);         
    $test = 'bug';
    $val  = 'bug';
  }

  function offsetexists($test) {
    var_dump($test);         
    $test = 'bug';
  }

  function offsetunset($test) {
    var_dump($test);         
    $test = 'bug';
  }

}

$foo = new Foo();
$a = "ok";

for ($i=0; $i < 2; $i++) {
  $foo->ok("ok");
  $foo->ok;
  $foo->ok = "ok";
  $x = $foo["ok"];
  $foo["ok"] = "ok";
  isset($foo["ok"]);
  unset($foo["ok"]);
//  $foo[];
  $foo[] = "ok";
//  isset($foo[]);
//  unset($foo[]);
  $foo->$a;
  echo "---\n";
}
?>
--EXPECT--
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
NULL
string(2) "ok"
string(2) "ok"
---
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
string(2) "ok"
NULL
string(2) "ok"
string(2) "ok"
---
