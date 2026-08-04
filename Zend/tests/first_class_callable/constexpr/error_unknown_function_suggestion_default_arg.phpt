--TEST--
FCC in default argument suggests a similar function name for a missing function.
--FILE--
<?php

function test(Closure $name = array_pussh(...)) {
    var_dump($name);
}

try {
    test();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
Call to undefined function array_pussh() (did you mean array_push()?)
