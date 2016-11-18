--TEST--
Bug #73478: openssl_pkey_new() should not generate a new public/private key pair for DH when the private key is provided
--DESCRIPTION--
The function openssl_pkey_new() should generate the public key when provided with
the private key, p and g for the key type Diffie-Hellman

DH_generate_key will generate public key if the private key is provided.
--SKIPIF--
<?php

if (!extension_loaded("openssl")) die("skip");
if (!extension_loaded("xml")) die("skip xml extension not loaded");
if (!@openssl_pkey_new()) die("skip cannot create private key");
?>
--FILE--
<?php

$dhparam = [
        'dh' => [
                'p' => base64_decode('0flGZE2YLsb4VEafVaKVXBJ/0UDuOyVRmpbDhfU8q0bp3LQ7EridPefJFaf4PU5mZUBUEiaxT9XI1xNRpuznJ0IdyfE04kzfmluhDsKGRplDbqpMfmVQjbsQO+Fb3rdMZqiO3jZEO7Xhi8V7oJUC8lcS12sn6hk5f7yDUVukufCWZ2nWummGIKfc2k9mbiKBVvp/LIu6StOf05tk0aN/Wt4lEn3p7tChgxDJFCeftVLp5I7Mwf6Iqa4hA833dVrST4Mq8S/OilAxVuCMliZuqgdwiqpZEkVQ8V4t8d6We3ojkbFuSWDhizUDG/0nSIeTAXIRfe8OjE8O/7iIPJSi0w=='),
                'g' => base64_decode('Ag=='),
                'priv_key' => base64_decode('QhxNavalH4TjlgKASyg2AvOoVVwZLu7y9Ng2dABWIgnj/dH4QyNZnxEpKoXWr2whNj43ANmPnliPBql7eHuSn6mBYK9kw8GT8aW8GSqzsjofulJRqHRpBvwTLYPlR6gdPTK6sTa8Bv/q9s0Hct1Ia0DnjU4TX5sQBKWf10eXCMxqhrpJKtXnZ1xXIt0e5+Fir/BX+Kv6QOvrY67LzZL29kwGCcvdpmLUT7uHzt2VnBxXkE38pMj/6FTAUtf/sUofB1hQZLF73bFZpknq3KT/j234XPhHbox084mLbCV9mXFPY6ZBDhWw6mEaP4k2oarkACSdAsag++69eIeXaIpSHA=='),
        ]
];

$keypair = openssl_pkey_new($dhparam);
$details = openssl_pkey_get_details($keypair);

echo "p: " . base64_encode($details['dh']['p']) . "\n";
echo "g: " . base64_encode($details['dh']['g']) . "\n";
echo "private key: " . base64_encode($details['dh']['priv_key']) . "\n";
echo "public key: " . base64_encode($details['dh']['pub_key']) . "\n";

?>
--EXPECT--
p: 0flGZE2YLsb4VEafVaKVXBJ/0UDuOyVRmpbDhfU8q0bp3LQ7EridPefJFaf4PU5mZUBUEiaxT9XI1xNRpuznJ0IdyfE04kzfmluhDsKGRplDbqpMfmVQjbsQO+Fb3rdMZqiO3jZEO7Xhi8V7oJUC8lcS12sn6hk5f7yDUVukufCWZ2nWummGIKfc2k9mbiKBVvp/LIu6StOf05tk0aN/Wt4lEn3p7tChgxDJFCeftVLp5I7Mwf6Iqa4hA833dVrST4Mq8S/OilAxVuCMliZuqgdwiqpZEkVQ8V4t8d6We3ojkbFuSWDhizUDG/0nSIeTAXIRfe8OjE8O/7iIPJSi0w==
g: Ag==
private key: QhxNavalH4TjlgKASyg2AvOoVVwZLu7y9Ng2dABWIgnj/dH4QyNZnxEpKoXWr2whNj43ANmPnliPBql7eHuSn6mBYK9kw8GT8aW8GSqzsjofulJRqHRpBvwTLYPlR6gdPTK6sTa8Bv/q9s0Hct1Ia0DnjU4TX5sQBKWf10eXCMxqhrpJKtXnZ1xXIt0e5+Fir/BX+Kv6QOvrY67LzZL29kwGCcvdpmLUT7uHzt2VnBxXkE38pMj/6FTAUtf/sUofB1hQZLF73bFZpknq3KT/j234XPhHbox084mLbCV9mXFPY6ZBDhWw6mEaP4k2oarkACSdAsag++69eIeXaIpSHA==
public key: FitxzHj4KRFoij43YQsyDxSbzQh9SigjI+BUmKg/UjjMYpiYvo3aOxNF8f9nDUsP5Ir4QNHeGYyVOhfeShZdXmXhUGHIao/lw3UmWnQBSxd7QHWxwensW1sx19g24gQYTYbYmupV/Hdc6hYa6pJ2P7eClabDsfrEeq2+hV/MZPuibVUBNOhuh3iszreYVNsf9LRDedZE33h43GQ7s73VhlYnhKCuSDB9SM1DLqhSvyvwPH1lnkyE1tVPoI6mg92sKeeqbVL7HTvnizDPHszPUNdkLnJAwMGxhw4zA/uUZDznLWOVwMHJbpsXobguC8qoeJ+mqkVNDhMqoCe6KZs+GA==

