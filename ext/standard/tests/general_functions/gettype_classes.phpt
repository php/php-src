--TEST--
Test gettype() class reading
--FILE--
<?php

class Test {

}

/* tests against an object type */
echo gettype(new Test()) . "\n";
echo gettype(new Test(), false) . "\n";
echo gettype(new Test(), true) . "\n";

/* tests against everything else */
echo gettype(1) . "\n";
echo gettype(1.1) . "\n";
echo gettype("foo") . "\n";
echo gettype(Test::class) . "\n";

--EXPECT--
object
object
Test
integer
double
string
string
