--TEST--
Closure with variadic type declaration
--FILE--
<?php
$f = function (StdClass ...$a) {
    var_dump($a);
};
$f(new StdClass);
?>
--EXPECT--
array(1) {
  [0]=>
  object(StdClass)#2 (0) {
  }
}
