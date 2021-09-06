--TEST--
$this on closure
--FILE--
<?php

class Test {}

$fn = function(): $this { return $this; };
var_dump($fn->call(new Test));

$fn2 = function(): $this { return new Test; };
try {
    var_dump($fn2->call(new Test));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump($fn2());
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
object(Test)#2 (0) {
}
Test::{closure}(): Return value must be of type $this, Test returned
{closure}(): Return value must be of type $this, Test returned
