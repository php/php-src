--TEST--
Base16 encode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base16;

use function Encoding\base16_encode;

$data = 'Hello world!';

echo base16_encode($data);
echo "\n";

echo base16_encode($data, variant: Base16::Lower);
echo "\n";

?>
--EXPECT--
48656C6C6F20776F726C6421
48656c6c6f20776f726c6421
