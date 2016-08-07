--TEST--
ReflectionMethod::invoke() with non object or null value
--FILE--
<?php

class a {
	function a(){
	}
}
class b {
}

$b = new b();

$a=new ReflectionClass("a");
$m=$a->getMethod("a");

try {
        $m->invoke(null);
} catch (ReflectionException $E) {
        echo $E->getMessage()."\n";
}


try {
        $m->invoke($b);
} catch (ReflectionException $E) {
        echo $E->getMessage()."\n";
}

$b = new a();
try {
        $m->invoke($b);
} catch (ReflectionException $E) {
        echo $E->getMessage()."\n";
}

echo "===DONE===\n";?>
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; a has a deprecated constructor in %s on line %d
Trying to invoke non static method a::a() without an object
Given object is not an instance of the class this method was declared in
===DONE===
