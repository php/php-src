--TEST--
Hash: hash_pbkdf2() basic functionality
--FILE--
<?php

echo "*** Testing hash_pbkdf2() basic functionality ***\n";
// Authoritative test vectors further below cover other functionality
echo "sha256(length 20): ", hash_pbkdf2("sha256", "password", "salt", 1, 20), "\n";
echo "sha256(no length): ", hash_pbkdf2("sha256", "password", "salt", 1), "\n";

echo "*** RFC 3962 ***\n"; // Just the 256-bit keys, to avoid pointless repetition
echo bin2hex(hash_pbkdf2("sha1", "password", "ATHENA.MIT.EDUraeburn", 1, 32, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", "password", "ATHENA.MIT.EDUraeburn", 2, 32, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", "password", "ATHENA.MIT.EDUraeburn", 1200, 32, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", "password", "\x12\x34\x56\x78\x78\x56\x34\x12", 5, 32, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", str_repeat("X", 64), "pass phrase equals block size", 1200, 32, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", str_repeat("X", 65), "pass phrase exceeds block size", 1200, 32, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", "\xf0\x9d\x84\x9e", "EXAMPLE.COMpianist", 50, 32, true)), "\n";

echo "*** RFC 6070 ***\n"; // Excluding the 2^24 rounds vector
echo bin2hex(hash_pbkdf2("sha1", "password", "salt", 1, 20, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", "password", "salt", 2, 20, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", "password", "salt", 4096, 20, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", "passwordPASSWORDpassword", "saltSALTsaltSALTsaltSALTsaltSALTsalt", 4096, 25, true)), "\n";
echo bin2hex(hash_pbkdf2("sha1", "pass\0word", "sa\0lt", 4096, 16, true)), "\n";

echo "*** RFC 7517 ***\n";
echo bin2hex(hash_pbkdf2("sha256", "Thus from my lips, by yours, my sin is purged.", hex2bin("50424553322d48533235362b413132384b5700d9609370967546f77f089b89ae2a50d7"), 4096, 16, true)), "\n";

echo "*** RFC 7914 ***\n";
echo bin2hex(hash_pbkdf2("sha256", "passwd", "salt", 1, 64, true)), "\n";
echo bin2hex(hash_pbkdf2("sha256", "Password", "NaCl", 80000, 64, true)), "\n";
?>
--EXPECT--
*** Testing hash_pbkdf2() basic functionality ***
sha256(length 20): 120fb6cffcf8b32c43e7
sha256(no length): 120fb6cffcf8b32c43e7225256c4f837a86548c92ccc35480805987cb70be17b
*** RFC 3962 ***
cdedb5281bb2f801565a1122b25635150ad1f7a04bb9f3a333ecc0e2e1f70837
01dbee7f4a9e243e988b62c73cda935da05378b93244ec8f48a99e61ad799d86
5c08eb61fdf71e4e4ec3cf6ba1f5512ba7e52ddbc5e5142f708a31e2e62b1e13
d1daa78615f287e6a1c8b120d7062a493f98d203e6be49a6adf4fa574b6e64ee
139c30c0966bc32ba55fdbf212530ac9c5ec59f1a452f5cc9ad940fea0598ed1
9ccad6d468770cd51b10e6a68721be611a8b4d282601db3b36be9246915ec82a
6b9cf26d45455a43a5b8bb276a403b39e7fe37a0c41e02c281ff3069e1e94f52
*** RFC 6070 ***
0c60c80f961f0e71f3a9b524af6012062fe037a6
ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957
4b007901b765489abead49d926f721d065a429c1
3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038
56fa6aa75548099dcc37d7f03425e0c3
*** RFC 7517 ***
6eaba95c815c6d75e9f274e9aa0e184b
*** RFC 7914 ***
55ac046e56e3089fec1691c22544b605f94185216dde0465e68b9d57c20dacbc49ca9cccf179b645991664b39d77ef317c71b845b1e30bd509112041d3a19783
4ddcd8f60b98be21830cee5ef22701f9641a4418d04c0414aeff08876b34ab56a1d425a1225833549adb841b51c9b3176a272bdebba1d078478f62b397f33c8d
