--TEST--
Test divisions functions : trows exception on zero denominator
--CREDITS--
Jean Carlo Machado <contato@jeancarlomachado.com.br>
--FILE--
<?php

$x = 5;
$y = 0;

try {
  $z = $x % $y;
} catch(Error $e) {
  print "% ".get_class($e)."\n";
}

try {
  $z = $x / $y;
} catch(Error $e) {
  print "/ ".get_class($e)."\n";
}

try {
  $x = intdiv((int)$x, $y);
} catch(Error $e) {
  print "intdiv ".get_class($e). "\n";
}

?>
--EXPECTF--
% DivisionByZeroError
/ DivisionByZeroError
intdiv DivisionByZeroError
