--TEST--
Passing assignments by reference
--FILE--
<?php

function f(&$a) {
  var_dump($a);
  $a = "a.changed";
} 

echo "\n\n---> Pass constant assignment by reference:\n";
f($a="a.original");
var_dump($a); 

echo "\n\n---> Pass variable assignment by reference:\n";
unset($a);
$a = "a.original";
f($b = $a);
var_dump($a); 

echo "\n\n---> Pass reference assignment by reference:\n";
unset($a, $b);
$a = "a.original";
f($b =& $a);
var_dump($a); 

echo "\n\n---> Pass concat assignment by reference:\n";
unset($a, $b);
$b = "b.original";
$a = "a.original";
f($b .= $a);
var_dump($a); 

?>
--EXPECTF--
---> Pass constant assignment by reference:

Strict Standards: Only variables should be passed by reference in %s on line %d
unicode(10) "a.original"
unicode(10) "a.original"


---> Pass variable assignment by reference:

Strict Standards: Only variables should be passed by reference in %s on line %d
unicode(10) "a.original"
unicode(10) "a.original"


---> Pass reference assignment by reference:
unicode(10) "a.original"
unicode(9) "a.changed"


---> Pass concat assignment by reference:

Strict Standards: Only variables should be passed by reference in %s on line %d
unicode(20) "b.originala.original"
unicode(10) "a.original"

