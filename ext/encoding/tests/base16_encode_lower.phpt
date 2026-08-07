--TEST--
Base16 encoding with Lower variant
--FILE--
<?php
use Encoding\Base16;
use function Encoding\base16_encode;

echo base16_encode('Hello world!', variant: Base16::Lower);
echo "\n";
?>
--EXPECT--
48656c6c6f20776f726c6421
