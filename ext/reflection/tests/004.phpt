--TEST--
ReflectionMethod::invoke() with non object or null value
--FILE--
<?php

class a {
    function __construct(){
    }
}
class b {
}

$b = new b();

$a=new ReflectionClass("a");
$m=$a->getMethod("__construct");

try {
        $m->invoke(null);
} catch (Throwable $E) {
        echo $E::class, ': ', $E->getMessage(), "\n";
}


try {
        $m->invoke($b);
} catch (Throwable $E) {
        echo $E::class, ': ', $E->getMessage(), "\n";
}

$b = new a();
try {
        $m->invoke($b);
} catch (Throwable $E) {
        echo $E::class, ': ', $E->getMessage(), "\n";
}

?>
--EXPECT--
ReflectionException: Trying to invoke non static method a::__construct() without an object
ReflectionException: Given object is not an instance of the class this method was declared in
