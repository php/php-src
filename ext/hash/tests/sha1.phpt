--TEST--
Hash: SHA1 algorithm
--FILE--
<?php
function test($data) {
	echo hash("sha1", $data), "\n";
}

test("");
test("a");

// FIPS 180-1 and 180-2 contain all three
// FIPS 180-3 and 180-4 link to a larger directory that only has the first two
//	https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
//	https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/SHA1.pdf
echo "*** FIPS 180-1 and later ***\n";
test("abc");
test("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq");
test(str_repeat("a", 1000000));

// IETF RFCs also include the FIPS samples from above
echo "*** RFC 3174 / RFC 4634 / RFC 6234 ***\n";
test(str_repeat("01234567012345670123456701234567", 20));
test("\x5e");
test("\x9a\x7d\xfd\xf1\xec\xea\xd0\x6e\xd6\x46\xaa\x55\xfe\x75\x71\x46");
test(
	"\xf7\x8f\x92\x14\x1b\xcd\x17\x0a\xe8\x9b\x4f\xba\x15\xa1\xd5\x9f\x3f\xd8\x4d\x22\x3c\x92\x51\xbd\xac\xbb\xae\x61\xd0\x5e\xd1\x15" .
	"\xa0\x6a\x7c\xe1\x17\xb7\xbe\xea\xd2\x44\x21\xde\xd9\xc3\x25\x92\xbd\x57\xed\xea\xe3\x9c\x39\xfa\x1f\xe8\x94\x6a\x84\xd0\xcf\x1f" .
	"\x7b\xee\xad\x17\x13\xe2\xe0\x95\x98\x97\x34\x7f\x67\xc8\x0b\x04\x00\xc2\x09\x81\x5d\x6b\x10\xa6\x83\x83\x6f\xd5\x56\x2a\x56\xca" .
	"\xb1\xa2\x8e\x81\xb6\x57\x66\x54\x63\x1c\xf1\x65\x66\xb8\x6e\x3b\x33\xa1\x08\xb0\x53\x07\xc0\x0a\xff\x14\xa7\x68\xed\x73\x50\x60" .
	"\x6a\x0f\x85\xe6\xa9\x1d\x39\x6f\x5b\x5c\xbe\x57\x7f\x9b\x38\x80\x7c\x7d\x52\x3d\x6d\x79\x2f\x6e\xbc\x24\xa4\xec\xf2\xb3\xa4\x27" .
	"\xcd\xbb\xfb"
);

// NIST SHAVS Monte Carlo test from RFC 6234
$seed = hex2bin('d0569cb3665a8a43eb6ea23d75a3c4d2054a0d7d');
for ($j = 0; $j < 4; $j++) {
	$md = [$seed, $seed, $seed];
	for ($i = 3; $i < 1003; $i++) {
		$md[$i] = hash("sha1", $md[$i-3].$md[$i-2].$md[$i-1], true);
	}
	$seed = $md[1002];
	echo bin2hex($seed), " (checkpoint {$j})\n";
}
?>
--EXPECT--
da39a3ee5e6b4b0d3255bfef95601890afd80709
86f7e437faa5a7fce15d1ddcb9eaeaea377667b8
*** FIPS 180-1 and later ***
a9993e364706816aba3e25717850c26c9cd0d89d
84983e441c3bd26ebaae4aa1f95129e5e54670f1
34aa973cd4c4daa4f61eeb2bdbad27316534016f
*** RFC 3174 / RFC 4634 / RFC 6234 ***
dea356a2cddd90c7a7ecedc5ebb563934f460452
5e6f80a34a9798cafc6a5db96cc57ba4c4db59c2
82abff6605dbe1c17def12a394fa22a82b544a35
cb0082c8f197d260991ba6a460e76e202bad27b3
e216836819477c7f78e0d843fe4ff1b6d6c14cd4 (checkpoint 0)
a2dbc7a5b1c6c0a8bcb7aaa41252a6a7d0690dbc (checkpoint 1)
db1f9050bb863dfef4ce37186044e2eeb17ee013 (checkpoint 2)
127fdedf43d372a51d5747c48fbffe38ef6cdf7b (checkpoint 3)
