--TEST--
Hash: HMAC-SHA3
--FILE--
<?php
function test($algo, $data, $key, $truncate = 0) {
	$result = hash_hmac($algo, $data, $key);
	($truncate > 0) && $result = substr($result, 0, $truncate * 2);
	echo "{$algo}: {$result}\n";
}

// HMAC with SHA3 is only meant for backwards-compatibility with MD-based hashes.
// As such, there are no official test vectors for HMAC-SHAKE (there's KMAC for that)
$algos = ["sha3-224", "sha3-256", "sha3-384", "sha3-512"];

// Samples from https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/example-values
echo "*** NIST samples ***\n";
foreach ($algos as $algo) {
	$blocklen = substr($algo, -3) / 8;
	test($algo, "Sample message for keylen<blocklen", pack("C*", ... range(0x00, $blocklen - 1)));
	test($algo, "Sample message for keylen=blocklen", pack("C*", ... range(0x00, 200 - ($blocklen * 2) - 1)));
	test($algo, "Sample message for keylen>blocklen", pack("C*", ... range(0x00, 200 - $blocklen - 1)));
	test($algo, "Sample message for keylen<blocklen, with truncated tag", pack("C*", ... range(0x00, $blocklen - 1)));
}

// These are truncated and full outputs aren't available
echo "*** SP 800-224 ipd ***\n";
test("sha3-224", hex2bin("7627b19cb55594587edad2ff0c22d292"), hex2bin("f8a7ed5562a7646a22b4dbb14d3ad891ca677877dae378602f09ce479d3b11e81a"), 11);
test("sha3-256", hex2bin("6d95ce1decc2212af7b33a90d6297e02"), hex2bin("5f712d90e610531aa24e2c5cb59b2b7f0e1d229809b10f46201e48d493eb6784ec"), 20);
test("sha3-384", hex2bin("c4222888afab77e7c9206d2894714e9a"), hex2bin("63e7020d5e017aa8f86618ba4a4ed4be03298e92ba8ef97c7396d26061b12d5d638c3e53ff1b8052b5e217a927eb7d9b80cedac1ceb227a13a0229df542f8b0f1040a5c8e9558cddeb"), 20);
test("sha3-512", hex2bin("676498a915cc5b773275034a972b552a"), hex2bin("a471b46143c47722a4317f79c3605f5606210066f7607f37bfc05ab48ad624ecddaa5f2bce0f5d68cb900a94041a388c"), 19);

?>
--EXPECT--
*** NIST samples ***
sha3-224: 332cfd59347fdb8e576e77260be4aba2d6dc53117b3bfb52c6d18c04
sha3-224: d8b733bcf66c644a12323d564e24dcf3fc75f231f3b67968359100c7
sha3-224: 078695eecc227c636ad31d063a15dd05a7e819a66ec6d8de1e193e59
sha3-224: 8569c54cbb00a9b78ff1b391b0e5cd2fa5ec728550aa3979703305d4
sha3-256: 4fe8e202c4f058e8dddc23d8c34e467343e23555e24fc2f025d598f558f67205
sha3-256: 68b94e2e538a9be4103bebb5aa016d47961d4d1aa906061313b557f8af2c3faa
sha3-256: 9bcf2c238e235c3ce88404e813bd2f3a97185ac6f238c63d6229a00b07974258
sha3-256: c8dc7148d8c1423aa549105dafdf9cad2941471b5c62207088e56ccf2dd80545
sha3-384: d588a3c51f3f2d906e8298c1199aa8ff6296218127f6b38a90b6afe2c5617725bc99987f79b22a557b6520db710b7f42
sha3-384: a27d24b592e8c8cbf6d4ce6fc5bf62d8fc98bf2d486640d9eb8099e24047837f5f3bffbe92dcce90b4ed5b1e7e44fa90
sha3-384: e5ae4c739f455279368ebf36d4f5354c95aa184c899d3870e460ebc288ef1f9470053f73f7c6da2a71bcaec38ce7d6ac
sha3-384: 25f4bf53606e91af79d24a4bb1fd6aecd44414a30c8ebb0ae09764c71aceefe8dfa72309e48152c98294be658a33836e
sha3-512: 4efd629d6c71bf86162658f29943b1c308ce27cdfa6db0d9c3ce81763f9cbce5f7ebe9868031db1a8f8eb7b6b95e5c5e3f657a8996c86a2f6527e307f0213196
sha3-512: 544e257ea2a3e5ea19a590e6a24b724ce6327757723fe2751b75bf007d80f6b360744bf1b7a88ea585f9765b47911976d3191cf83c039f5ffab0d29cc9d9b6da
sha3-512: 5f464f5e5b7848e3885e49b2c385f0694985d0e38966242dc4a5fe3fea4b37d46b65ceced5dcf59438dd840bab22269f0ba7febdb9fcf74602a35666b2a32915
sha3-512: 7bb06d859257b25ce73ca700df34c5cbef5c898bac91029e0b27975d4e526a088f5e590ee736969f445643a58bee7ee0cbbbb2e14775584435d36ad0de6b9499
*** SP 800-224 ipd ***
sha3-224: 1af28609d217bf6dfb1184
sha3-256: ed29d0d3923524ae417f0b30dff8a4128dc202ae
sha3-384: 0b546df3ef91e1da09e5e7efc7258ca2da57cbe6
sha3-512: cf38aa4b510886a34fb3b67f50f8fed59de585
