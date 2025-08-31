--TEST--
Phar: unable to process zip (zip spanning multiple archives)
--EXTENSIONS--
phar
--FILE--
<?php
try {
    new PharData(__DIR__ . '/files/disknumber.zip');
} catch (Exception $e) {
    echo $e->getMessage() . "\n";
}
?>
--EXPECTF--
phar error: split archives spanning multiple zips cannot be processed in zip-based phar "%sdisknumber.zip"
