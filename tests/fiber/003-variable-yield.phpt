--TEST--
tests Fiber::yield variable
--FILE--
<?php
$f = new Fiber(function () {
    Fiber::yield("string");
    Fiber::yield(1);
    Fiber::yield([1, 2]);
    Fiber::yield(new stdclass);
});
var_dump($f->resume());
var_dump($f->resume());
var_dump($f->resume());
var_dump($f->resume());
?>
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
