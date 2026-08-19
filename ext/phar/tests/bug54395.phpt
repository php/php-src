--TEST--
Bug #54395 (Phar::mount() crashes when calling with wrong parameters)
--EXTENSIONS--
phar
--FILE--
<?php

try {
    Phar::mount(1,1);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}

?>
--EXPECT--
PharException: Mounting of 1 to 1 failed
