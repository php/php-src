--TEST--
Closure with variadic type hint
--FILE--
<?php
$f = function (stdClass ...$a) {
    var_dump($a);
};
$f(new stdClass);
?>
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#2 (0) {
  }
}
