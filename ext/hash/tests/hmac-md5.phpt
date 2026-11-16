--TEST--
Hash: HMAC-MD5
--FILE--
<?php
echo "*** RFC 2104 / RFC 2202 ***\n";
echo hash_hmac("md5", "Hi There", str_repeat("\x0b", 16)), "\n";
echo hash_hmac("md5", "what do ya want for nothing?", "Jefe"), "\n";
echo hash_hmac("md5", str_repeat("\xdd", 50), str_repeat("\xaa", 16)), "\n";
echo hash_hmac("md5", str_repeat("\xcd", 50), pack("C*", ... range(0x01, 0x19))), "\n";
echo hash_hmac("md5", "Test With Truncation", str_repeat("\x0c", 16)), "\n";
echo hash_hmac("md5", "Test Using Larger Than Block-Size Key - Hash Key First", str_repeat("\xaa", 80)), "\n";
echo hash_hmac("md5", "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data", str_repeat("\xaa", 80)), "\n";
?>
--EXPECT--
*** RFC 2104 / RFC 2202 ***
9294727a3638bb1c13f48ef8158bfc9d
750c783e6ab0b503eaa86e310a5db738
56be34521d144c88dbb8c733f0e8b3f6
697eaf0aca3a3aea3a75164746ffaa79
56461ef2342edc00f9bab995690efd4c
6b1ab7fe4bd7bf8f0b62e6ce61b9d0cd
6f630fad67cda0ee1fb1f562db3aa53e
