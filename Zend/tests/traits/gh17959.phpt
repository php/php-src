--TEST--
GH-17959: Missing trait error is recoverable
--FILE--
<?php

try {
    class C {
        use MissingTrait;
    }
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
===DONE===
--EXPECT--
Error: Trait "MissingTrait" not found
===DONE===
