--TEST--
Base32 encode
--EXTENSIONS--
encoding
--FILE--
<?php

use Encoding\Base32;
use Encoding\PaddingMode;

use function Encoding\base32_encode;

$data = 'Hello world!';

echo base32_encode($data);
echo "\n";

echo base32_encode($data, variant: Base32::Ascii);
echo "\n";

echo base32_encode($data, paddingMode: PaddingMode::StripPadding);
echo "\n";

echo base32_encode($data, variant: Base32::Crockford);
echo "\n";

?>
--EXPECT--
JBSWY3DPEB3W64TMMQQQ====
JBSWY3DPEB3W64TMMQQQ====
JBSWY3DPEB3W64TMMQQQ
91JPRV3F41VPYWKCCGGG
