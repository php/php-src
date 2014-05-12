--TEST--
RFC 2144 test vectors for CAST-128 in ECB mode (bug #62102)
--SKIPIF--
<?php extension_loaded('mcrypt') OR print 'skip'; ?>
--FILE--
<?php
$plaintext = "\x01\x23\x45\x67\x89\xAB\xCD\xEF";

echo

"128-bit: ",
bin2hex(mcrypt_encrypt('cast-128', "\x01\x23\x45\x67\x12\x34\x56\x78\x23\x45\x67\x89\x34\x56\x78\x9A", $plaintext, 'ecb')),
"\n",
"80-bit: ",
bin2hex(mcrypt_encrypt('cast-128', "\x01\x23\x45\x67\x12\x34\x56\x78\x23\x45", $plaintext, 'ecb')),
"\n",
"40-bit: ",
bin2hex(mcrypt_encrypt('cast-128', "\x01\x23\x45\x67\x12", $plaintext, 'ecb')),
"\n";

?>
--EXPECTF--
128-bit: 238b4fe5847e44b2
80-bit: 
Warning: mcrypt_encrypt(): Key of size 10 not supported by this algorithm. Only keys of size 16 supported in %s on line %d

40-bit: 
Warning: mcrypt_encrypt(): Key of size 5 not supported by this algorithm. Only keys of size 16 supported in %s on line %d
