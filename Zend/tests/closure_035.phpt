--TEST--
Testing recursion detection with Closures
--FILE--
<?php

$x = function () use (&$x) {
    $h = function () use ($x) {
        var_dump($x);
        return 1;
    };
    return $h();
};

var_dump($x());

?>
--EXPECTF--
object(Closure)#%d (2) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["static"]=>
  array(1) {
    ["x"]=>
    *RECURSION*
  }
}
int(1)
