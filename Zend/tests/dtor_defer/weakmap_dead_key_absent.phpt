--TEST--
Deferred destructor: a dying WeakMap key leaves the map at logical death
--FILE--
<?php
$map = new WeakMap;
$k1 = new stdClass;
$k2 = new stdClass;
$map[$k1] = 1;
$map[$k2] = 2;
echo "before: ", count($map), "\n";
unset($k1);
echo "after unset k1: ", count($map), "\n";
echo "end\n";
?>
--EXPECT--
before: 2
after unset k1: 1
end
