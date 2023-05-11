--TEST--
Hash: hash_file() function : basic functionality
--FILE--
<?php

echo "*** Testing hash_file() : basic functionality ***\n";

$file = __DIR__ . "hash_file.txt";
/* Creating a temporary file file */
if (($fp = fopen( $file, "w+")) == FALSE) {
    echo "Cannot create file ($file)";
    exit;
}

/* Writing into file */
$content = "This is a sample string used to test the hash_file function with various hashing algorithms";
if (is_writable($file)) {
  if (fwrite($fp, $content) === FALSE) {
    echo "Cannot write to file ($file)";
    exit;
  }
}

// close the file
fclose($fp);

echo "adler32: " . hash_file('adler32', $file) . "\n";
echo "crc32: " . hash_file('crc32', $file) . "\n";
echo "gost: " . hash_file('gost', $file). "\n";
echo "haval128,3: " . hash_file('haval128,3', $file). "\n";
echo "md2: " . hash_file('md2', $file). "\n";
echo "md4: " . hash_file('md4', $file). "\n";
echo "md5: " . hash_file('md5', $file). "\n";
echo "ripemd128: " . hash_file('ripemd128', $file). "\n";
echo "ripemd160: " . hash_file('ripemd160', $file). "\n";
echo "ripemd256: " . hash_file('ripemd256', $file). "\n";
echo "ripemd320: " . hash_file('ripemd320', $file). "\n";
echo "sha1: " . hash_file('sha1', $file). "\n";
echo "sha256: " . hash_file('sha256', $file). "\n";
echo "sha384: " . hash_file('sha384', $file). "\n";
echo "sha512: " . hash_file('sha512', $file). "\n";
echo "snefru: " . hash_file('snefru', $file). "\n";
echo "tiger192,3: " . hash_file('tiger192,3', $file). "\n";
echo "whirlpool: " . hash_file('whirlpool', $file). "\n";
echo "murmur3a: " . hash_file('murmur3a', $file). "\n";
echo "murmur3a: " . hash_file('murmur3a', $file, false, ['seed' => 1234]). "\n";
echo "murmur3c: " . hash_file('murmur3c', $file). "\n";
echo "murmur3c: " . hash_file('murmur3c', $file, false, ['seed' => 1234]). "\n";
echo "murmur3f: " . hash_file('murmur3f', $file). "\n";
echo "murmur3f: " . hash_file('murmur3f', $file, false, ['seed' => 1234]). "\n";

echo "adler32(raw): " . bin2hex(hash_file('adler32', $file, TRUE)) . "\n";
echo "md5(raw): " . bin2hex(hash_file('md5', $file, TRUE)). "\n";
echo "sha256(raw): " . bin2hex(hash_file('sha256', $file, TRUE)). "\n";

unlink($file);

?>
--EXPECT--
*** Testing hash_file() : basic functionality ***
adler32: ff87222e
crc32: 61664d33
gost: d9e65f0c0c2ef944e4f8a01f4a46365c4f33a2853756878182a7f03e1490a4cd
haval128,3: c25962b13383c3ed9f13817c8f2ae7d6
md2: 70f791c0d8fa9edd7d08e32fcba8c354
md4: a9d034b16bb290c57a645afd6f14cd3b
md5: 704bf818448f5bbb94061332d2c889aa
ripemd128: d02a5f320a11c54c7d51f933b0bd8471
ripemd160: 3ff296ca6314313af3ed0437c8fc0ebbd3242d3b
ripemd256: 0edd779587c11cf32781111b264251eb37529832fb207121cd45dd95002e48a8
ripemd320: bf162fa2ff20491b3016c5d8190f8ee47d7dcda8c38eaf6779349a243a029d275eec9adf16ec1b35
sha1: 8529b266611e3bd0d208fd9614653c2a8f23d0fe
sha256: a0f5702fa5d3670b80033d668e8732b70550392abb53841355447f8bb0f72245
sha384: a35d875ed96d94b6452acad910f97978200faa2398d8a0e6b9cffa33704c3809e3d2e5b0d63700d8f32a0716e7d2d528
sha512: 1f42adaf938fbf136e381b164bae5f984c7f9fe60c82728bd889c14f187c7d63e81a0305a1731c7e0a8f3ed9fd2ec92a3833a93502bdf269532601f0b8e2bab0
snefru: d414b2345d3e7fa1a31c044cf334bfc1fec24d89e464411998d579d24663895f
tiger192,3: 7acf4ebea075fac6fc8ea0e2b4af3cfa71b9460e4c53403a
whirlpool: 4248b149e000477269a4a5f1a84d97cfc3d0199b7aaf505913e6f010a6f83276029d11a9ad545374bc710eb59c7d958985023ab886ffa9ec9a23852844c764ec
murmur3a: bc6554c8
murmur3a: 432e4379
murmur3c: 8779de509ffc06fb27bcf5fc861504d6
murmur3c: b43afac65c38a617323020432c170005
murmur3f: 2b84cd546b2f18a9ab6f893194224afd
murmur3f: 6cc7716646664d6a83d68cb6563ac38e
adler32(raw): ff87222e
md5(raw): 704bf818448f5bbb94061332d2c889aa
sha256(raw): a0f5702fa5d3670b80033d668e8732b70550392abb53841355447f8bb0f72245
