--TEST--
The Closure object of a generator is freed when the generator is freed.
--FILE--
<?php

$genFactory = function() {
  yield 1;
  yield 2;
  yield 3;
};

$r = WeakReference::create($genFactory);
$generator = $genFactory();
unset($genFactory);

var_dump($r->get());

foreach ($generator as $value) var_dump($value);

var_dump($r->get());

unset($generator);

var_dump($r->get());

?>
--EXPECTF--
object(Closure)#%d (3) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}
int(1)
int(2)
int(3)
object(Closure)#%d (3) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}
NULL
