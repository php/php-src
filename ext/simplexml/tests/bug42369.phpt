--TEST--
Bug #42369 (Implicit conversion to string leaks memory)
--SKIPIF--
<?php if (!extension_loaded('simplexml')) echo 'skip simplexml extension is not loaded'; ?>
--FILE--
<?php
    $xml = '<?xml version="1.0" encoding="utf-8"?>';
    $x = simplexml_load_string($xml . "<q><x>foo</x></q>");

    echo 'explicit conversion' . PHP_EOL;
    for ($i = 0; $i < 100000; $i++) {
        md5(strval($x->x));
    }

    echo 'no conversion' . PHP_EOL;
    for ($i = 0; $i < 100000; $i++) {
        md5($x->x);
    }

?>
===DONE===
--EXPECT--
explicit conversion
no conversion
===DONE===