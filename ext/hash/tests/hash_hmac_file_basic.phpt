--TEST--
Hash: hash_hmac_file() basic functionality
--FILE--
<?php
echo "*** Testing hash_hmac_file() basic functionality ***\n";

$hash = hash_hmac_file("md5", __FILE__, "secret");
echo "Default output is hex-encoded: ", var_export((bool) strspn($hash, "0123456789abcdef"), true), "\n";
$hex = hash_hmac_file("md5", __FILE__, "secret", false);
$bin = hash_hmac_file("md5", __FILE__, "secret", true);
echo "Hex output equals bin2hex(binary): ", var_export($hex === bin2hex($bin), true), "\n";

$data = include(__DIR__."/samples.inc");
list($message, $digest) = [$data["message"], $data["digest"]];
$file = __DIR__."hash_hmac_file.txt";
if (false === file_put_contents($file, $message)) {
	echo "ERROR: Cannot create file {$file}\n";
	exit;
}

$data = include(__DIR__."/samples.inc");
list($message, $key, $hmac) = [$data["message"], $data["key"], $data["hmac"]];
echo "*** Outputs for content '{$message}' with key '{$key}' ***\n";
foreach (hash_hmac_algos() as $algo) {
	echo "{$algo}: ", $hash = hash_hmac_file($algo, $file, $key), "\n";
	if ($hash !== $hmac[$algo]) {
		echo "{$algo} EXPECTED {$hmac[$algo]}\n";
	}
}
unlink($file);
?>
--EXPECT--
*** Testing hash_hmac_file() basic functionality ***
Default output is hex-encoded: true
Hex output equals bin2hex(binary): true
*** Outputs for content 'The quick brown fox jumps over the lazy dog' with key 'key' ***
md2: 13758b9534bfb38d850457814613b0c1
md4: 8d3366c440a9c65124ab0b5f4ca27338
md5: 80070713463e7749b90c2dc24911e275
sha1: de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9
sha224: 88ff8b54675d39b8f72322e65ff945c52d96379988ada25639747e69
sha256: f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8
sha384: d7f4727e2c0b39ae0f1e40cc96f60242d5b7801841cea6fc592c5d3e1ae50700582a96cf35e1e554995fe4e03381c237
sha512/224: a1afb4f708cb63570639195121785ada3dc615989cc3c73f38e306a3
sha512/256: 7fb65e03577da9151a1016e9c2e514d4d48842857f13927f348588173dca6d89
sha512: b42af09057bac1e2d41708e48a902e09b5ff7f12ab428a4fe86653c73dd248fb82f948a549f7b791a5b41915ee4d1ec3935357e4e2317250d0372afa2ebeeb3a
sha3-224: ff6fa8447ce10fb1efdccfe62caf8b640fe46c4fb1007912bf85100f
sha3-256: 8c6e0683409427f8931711b10ca92a506eb1fafa48fadd66d76126f47ac2c333
sha3-384: aa739ad9fcdf9be4a04f06680ade7a1bd1e01a0af64accb04366234cf9f6934a0f8589772f857681fcde8acc256091a2
sha3-512: 237a35049c40b3ef5ddd960b3dc893d8284953b9a4756611b1b61bffcf53edd979f93547db714b06ef0a692062c609b70208ab8d4a280ceee40ed8100f293063
ripemd128: ea830b2f823e559e753aecfa22cf666c
ripemd160: 50278a77d4d7670561ab72e867383aef6ce50b3e
ripemd256: 39f102599868d204bbf6165139f79eaa856a75cf92d785492907e2fee4168097
ripemd320: dfca8756189fc556323fb344001a927c161f83a9d8f402d092c537346ae977113c4d02cca757a7ad
whirlpool: 7f7192e3a155cb6a8171584ba146882f26821658112dfd2601272db013517a31e573637d146584596f86a884eb0decc9514dde000ecf2476dc5d436a92197527
tiger128,3: eb8aa51180c56e53873f7fccd6b1ed03
tiger160,3: 7be9669d759b97704005ec3515c7f7c856f70b9e
tiger192,3: fba544227e1471d8d47dd9d68c5008d554c6de6072ca2e2a
tiger128,4: b96c7efb6b1aabda1ea6a3f8ae59811a
tiger160,4: 02e8155ee2b6f84f5c288c54b030bd40ae327fe9
tiger192,4: 65e3808ac6672d2caefa1a8e7fa0f707686291401c63dfda
snefru: f06bce8a92372a56c5e188fd4151c62f3f420edb7bbec5481bb08a4b89ca7b46
snefru256: f06bce8a92372a56c5e188fd4151c62f3f420edb7bbec5481bb08a4b89ca7b46
gost: 71be5a01e820943523a70bfeaa139189a1c70dd05cfcd3467cba3694667dfdcb
gost-crypto: e06ac9388fa2107fa7bb49d6b29c28a09a2c0cde316cd349a12bb4b0d3497370
haval128,3: b2648a98efd89b3c1995a4d622003f47
haval160,3: 18f111448d2c12521624d90e3554682a96a6cc49
haval192,3: c49d5cb9adcce283761052f0a6d9c56a55b4f5c3c1fb6dc1
haval224,3: 15e86138589cf2e5caf5af4247347df591ae054b861612518ddd502e
haval256,3: 1f07b3feac9df53307bc97c3ece4ce487266746987c65ef094d15e03bca1fc26
haval128,4: fe0c0f63b4fb9b187d96ff0a6cd77d57
haval160,4: daab2d68842f29241a6b7303444828e0ba707b36
haval192,4: c19ec834a5f2dedbb0279b1ee4cd5054dc414c704568d375
haval224,4: 7ed43b91cef3c3dc1d629c2029df2d701c1ade64f9d921e0c12cc2ca
haval256,4: 565896a45b9711597969a85772ecd1b7b45e84377874d25349239aa22ab54323
haval128,5: 96e68e4253a30ca782a2c3e84b14cd15
haval160,5: 5e0c69785712da1bf4060a3fe575fb4e5a5dd2a0
haval192,5: 284c96f4926e9a0358c7a648a885b79df29bb84293ac5ad1
haval224,5: 736e38e52579198cb64a7b0dabab1158954c156d3a38817a2f3b5868
haval256,5: 6c22f28a4705da5b8a963edd11dc7adb7a8b974bc1f3109718de44581f71102e
