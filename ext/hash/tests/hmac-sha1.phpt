--TEST--
Hash: HMAC-SHA1
--FILE--
<?php
function test($data, $key) {
	echo hash_hmac("sha1", $data, $key), "\n";
}

// FIPS 198
echo "*** FIPS 198 ***\n";
test("Sample #1", pack("C*", ... range(0x00, 0x3f)));
test("Sample #2", pack("C*", ... range(0x30, 0x43)));
test("Sample #3", pack("C*", ... range(0x50, 0xb3)));
test("Sample #4", pack("C*", ... range(0x70, 0xa0)));

// FIPS 198-1 and later link to a larger directory
//	https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
//	https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/HMAC_SHA1.pdf
echo "*** FIPS 198-1 and later ***\n";
test("Sample message for keylen=blocklen", pack("C*", ... range(0x00, 0x3f)));
test("Sample message for keylen<blocklen", pack("C*", ... range(0x00, 0x13)));
test("Sample message for keylen=blocklen", pack("C*", ... range(0x00, 0x63)));
test("Sample message for keylen<blocklen, with truncated tag", pack("C*", ... range(0x00, 0x30)));

echo "*** RFC 2202 / RFC 4634 / RFC 6234 ***\n";
test("Hi There", str_repeat("\x0b", 20));
test("what do ya want for nothing?", "Jefe");
test(str_repeat("\xdd", 50), str_repeat("\xaa", 20));
test(str_repeat("\xcd", 50), pack("C*", ... range(0x01, 0x19)));
test("Test With Truncation", str_repeat("\x0c", 20));
test("Test Using Larger Than Block-Size Key - Hash Key First", str_repeat("\xaa", 80));
test("Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data", str_repeat("\xaa", 80));
?>
--EXPECT--
*** FIPS 198 ***
4f4ca3d5d68ba7cc0a1208c9c61e9c5da0403c0a
0922d3405faa3d194f82a45830737d5cc6c75d24
bcf41eab8bb2d802f3d05caf7cb092ecf8d1a3aa
9ea886efe268dbecce420c7524df32e0751a2a26
*** FIPS 198-1 and later ***
5fd596ee78d5553c8ff4e72d266dfd192366da29
4c99ff0cb1b31bd33f8431dbaf4d17fcd356a807
2d51b2f7750e410584662e38f133435f4c4fd42a
fe3529565cd8e28c5fa79eac9d8023b53b289d96
*** RFC 2202 / RFC 4634 / RFC 6234 ***
b617318655057264e28bc0b6fb378c8ef146be00
effcdf6ae5eb2fa2d27416d5f184df9c259a7c79
125d7342b9ac11cd91a39af48aa17b4f63f175d3
4c9007f4026250c6bc8414f9bf50c86c2d7235da
4c1a03424b55e07fe7f27be1d58bb9324a9a5a04
aa4ae5e15272d00e95705637ce8a3b55ed402112
e8e99d0f45237d786d6bbaa7965c7808bbff1a91
