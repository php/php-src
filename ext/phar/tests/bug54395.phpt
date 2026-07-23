--TEST--
Bug #54395 (Phar::mount() crashes when calling with wrong parameters)
--EXTENSIONS--
phar
--FILE--
<?php

try {
    phar::mount(1,1);
} catch (Exception $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
PharException: Mounting of 1 to 1 failed
