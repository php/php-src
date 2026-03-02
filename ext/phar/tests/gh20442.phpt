--TEST--
GH-20442 (Phar does not respect case-insensitiveness of __halt_compiler() when reading stub)
--EXTENSIONS--
phar
--FILE--
<?php

$phar = new Phar(__DIR__.'/files/gh20442.phar');
var_dump($phar->count());
var_dump($phar->getStub());

?>
--EXPECT--
int(1)
string(50) "<?php
echo "Hello World!";
__halt_compiler(); ?>
"
