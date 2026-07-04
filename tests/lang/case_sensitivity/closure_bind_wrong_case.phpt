--TEST--
Closure::bind() and bindTo() with wrong-case scope fails with wrong case
--FILE--
<?php
class MyScope {
    private int $x = 1;
}

$f = function() { return $this->x; };

$bound1 = Closure::bind($f, new MyScope, "myscope");
echo $bound1() . "\n";

$bound2 = $f->bindTo(new MyScope, "MYSCOPE");
echo $bound2() . "\n";
?>
--EXPECTF--
Warning: Class "myscope" not found in %s on line %d

Fatal error: Uncaught Error: Value of type null is not callable in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
