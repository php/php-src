--TEST--
invoke with non object or null value
--FILE--
<?php

include_once dirname(__FILE__).'/exception.php';

class a {
	function a(){
	}
}
class b {
}

$b = new b();

$a=new Reflection_Class("a");
$m=$a->getMethod("a");

try {
        $m->invoke(null);
} catch (reflection_exception $E) {
        echo $E->getMessage()."\n";
}


try {
        $m->invoke($b);
} catch (reflection_exception $E) {
        echo $E->getMessage()."\n";
}

$b = new a();
try {
        $m->invoke($b);
} catch (reflection_exception $E) {
        echo $E->getMessage()."\n";
}

echo "===DONE===\n";?>
--EXPECT--
Non-object passed to Invoke()
Given object is not an instance of the class this method was declared in
===DONE===
