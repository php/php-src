--TEST--
Base85 encode Z85 variant
--FILE--
<?php
use Encoding\Base85;
use function Encoding\base85_encode;

echo base85_encode('Hello world!', variant: Base85::Z85);
echo "\n";
?>
--EXPECT--
nm=QNz.92Pz/P
