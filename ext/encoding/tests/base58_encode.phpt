--TEST--
Base58 encode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base58;

use function Encoding\base58_encode;

$data = 'Hello world!';

echo base58_encode($data);
echo "\n";

echo base58_encode($data, variant: Base58::Bitcoin);
echo "\n";

echo base58_encode($data, variant: Base58::Flickr);
echo "\n";

?>
--EXPECT--
2NEpo7TZRhna7vSvL
2NEpo7TZRhna7vSvL
2nePN7syqGMz7VrVk
