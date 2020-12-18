--TEST--
Check for libsodium box
--SKIPIF--
<?php if (!extension_loaded("sodium")) print "skip"; ?>
--FILE--
<?php
$keypair = sodium_crypto_box_keypair();
var_dump(strlen($keypair) === SODIUM_CRYPTO_BOX_KEYPAIRBYTES);
$sk = sodium_crypto_box_secretkey($keypair);
var_dump(strlen($sk) === SODIUM_CRYPTO_BOX_SECRETKEYBYTES);
$pk = sodium_crypto_box_publickey($keypair);
var_dump(strlen($pk) === SODIUM_CRYPTO_BOX_PUBLICKEYBYTES);
var_dump($pk !== $sk);
$pk2 = sodium_crypto_box_publickey_from_secretkey($sk);
var_dump($pk === $pk2);
$pk2 = sodium_crypto_scalarmult_base($sk);
var_dump($pk === $pk2);
$keypair2 = sodium_crypto_box_keypair_from_secretkey_and_publickey($sk, $pk);
var_dump($keypair === $keypair2);

$seed_x = str_repeat('x', SODIUM_CRYPTO_BOX_SEEDBYTES);
$seed_y = str_repeat('y', SODIUM_CRYPTO_BOX_SEEDBYTES);
$alice_box_kp = sodium_crypto_box_seed_keypair($seed_x);
$bob_box_kp = sodium_crypto_box_seed_keypair($seed_y);
$message_nonce = random_bytes(SODIUM_CRYPTO_BOX_NONCEBYTES);

$alice_box_secretkey = sodium_crypto_box_secretkey($alice_box_kp);
$bob_box_publickey = sodium_crypto_box_publickey($bob_box_kp);

$alice_to_bob_kp = sodium_crypto_box_keypair_from_secretkey_and_publickey(
    $alice_box_secretkey,
    $bob_box_publickey
);

$msg = "Here is another message, to be signed using Alice's secret key, and " .
  "to be encrypted using Bob's public key. The keys will always be the same " .
  "since they are derived from a fixed seeds";

$ciphertext = sodium_crypto_box(
    $msg,
    $message_nonce,
    $alice_to_bob_kp
);

try {
  $ciphertext = sodium_crypto_box(
      $msg,
      $message_nonce,
      substr($alice_to_bob_kp, 1)
  );
} catch (SodiumException $ex) {
    echo $ex->getMessage(), PHP_EOL;
}

sodium_memzero($alice_box_kp);
sodium_memzero($bob_box_kp);

$alice_box_kp = sodium_crypto_box_seed_keypair($seed_x);
$bob_box_kp = sodium_crypto_box_seed_keypair($seed_y);

$alice_box_publickey = sodium_crypto_box_publickey($alice_box_kp);
$bob_box_secretkey = sodium_crypto_box_secretkey($bob_box_kp);

$bob_to_alice_kp = sodium_crypto_box_keypair_from_secretkey_and_publickey(
    $bob_box_secretkey,
    $alice_box_publickey
);

$plaintext = sodium_crypto_box_open(
    $ciphertext,
    $message_nonce,
    $bob_to_alice_kp
);

var_dump($msg === $plaintext);

$alice_kp = sodium_crypto_box_keypair();
$alice_secretkey = sodium_crypto_box_secretkey($alice_kp);
$alice_publickey = sodium_crypto_box_publickey($alice_kp);

$bob_kp = sodium_crypto_box_keypair();
$bob_secretkey = sodium_crypto_box_secretkey($bob_kp);
$bob_publickey = sodium_crypto_box_publickey($bob_kp);

$alice_to_bob_kp = sodium_crypto_box_keypair_from_secretkey_and_publickey
  ($alice_secretkey, $bob_publickey);

$bob_to_alice_kp = sodium_crypto_box_keypair_from_secretkey_and_publickey
  ($bob_secretkey, $alice_publickey);

$alice_to_bob_message_nonce = random_bytes(SODIUM_CRYPTO_BOX_NONCEBYTES);

$alice_to_bob_ciphertext = sodium_crypto_box('Hi, this is Alice',
                                              $alice_to_bob_message_nonce,
                                              $alice_to_bob_kp);

$alice_message_decrypted_by_bob = sodium_crypto_box_open($alice_to_bob_ciphertext,
                                                          $alice_to_bob_message_nonce,
                                                          $bob_to_alice_kp);

$bob_to_alice_message_nonce = random_bytes(SODIUM_CRYPTO_BOX_NONCEBYTES);

$bob_to_alice_ciphertext = sodium_crypto_box('Hi Alice! This is Bob',
                                              $bob_to_alice_message_nonce,
                                              $bob_to_alice_kp);

$bob_message_decrypted_by_alice = sodium_crypto_box_open($bob_to_alice_ciphertext,
                                                          $bob_to_alice_message_nonce,
                                                          $alice_to_bob_kp);

var_dump($alice_message_decrypted_by_bob);
var_dump($bob_message_decrypted_by_alice);

if (SODIUM_LIBRARY_MAJOR_VERSION > 7 ||
    (SODIUM_LIBRARY_MAJOR_VERSION == 7 &&
     SODIUM_LIBRARY_MINOR_VERSION >= 5)) {
    $anonymous_message_to_alice = sodium_crypto_box_seal('Anonymous message',
                                                          $alice_publickey);

    $decrypted_message = sodium_crypto_box_seal_open($anonymous_message_to_alice,
                                                      $alice_kp);
} else {
    $decrypted_message = 'Anonymous message';
}
var_dump($decrypted_message);

$msg = sodium_hex2bin(
    '7375f4094f1151640bd853cb13dbc1a0ee9e13b0287a89d34fa2f6732be9de13f88457553d'.
    '768347116522d6d32c9cb353ef07aa7c83bd129b2bb5db35b28334c935b24f2639405a0604'
);
$kp = sodium_hex2bin(
    '36a6c2b96a650d80bf7e025e0f58f3d636339575defb370801a54213bd54582d'.
    '5aecbcf7866e7a4d58a6c1317e2b955f54ecbe2fcbbf7d262c10636ed524480c'
);
var_dump(sodium_crypto_box_seal_open($msg, $kp));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
sodium_crypto_box(): Argument #3 ($key_pair) must be SODIUM_CRYPTO_BOX_KEYPAIRBYTES bytes long
bool(true)
string(17) "Hi, this is Alice"
string(21) "Hi Alice! This is Bob"
string(17) "Anonymous message"
string(26) "This is for your eyes only"
