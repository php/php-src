--TEST--
tests Fiber await variable
--FILE--
<?php
$f = new Fiber(function () {
    await "string";
    await 1;
    await [1, 2];
    await new stdclass;
});
var_dump($f->resume());
var_dump($f->resume());
var_dump($f->resume());
var_dump($f->resume());
--EXPECTF--
string(6) "string"
int(1)
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}
object(stdClass)#%d (0) {
}
