--TEST--
Base16 encoding default (uppercase)
--FILE--
<?php
use function Encoding\base16_encode;

echo base16_encode('Hello world!');
echo "\n";
?>
--EXPECT--
48656C6C6F20776F726C6421
