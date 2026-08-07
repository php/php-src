--TEST--
Bug #51791 (constant() failed to check undefined constant and php interpreter stopped)
--FILE--
<?php

class A  {
    const B = 1;
}
try {
    constant('A::B1');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
Error: Undefined constant A::B1
