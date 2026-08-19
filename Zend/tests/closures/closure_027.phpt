--TEST--
Closure 027: Testing Closure type-hint
--FILE--
<?php

function test(Closure $a) {
    var_dump($a());
}


test(function() { return new stdclass; });

test(function() { });

$a = function($x) use ($y) {};
try {
    test($a);
} catch (Throwable $e) {
    echo "Exception: " . $e->getMessage() . "\n";
}

test(new stdclass);

?>
--EXPECTF--
Fatal error: Uncaught Error: Class "stdclass" not found in %s:%d
Stack trace:
#%d %s(4): {closure:%s:%d}()
#%d %s(8): test(Object(Closure))
#%d {main}
  thrown in %s on line %d
