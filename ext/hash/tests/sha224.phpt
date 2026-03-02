--TEST--
Hash: sha224 algorithm
--FILE--
<?php
echo hash('sha224', '') . "\n";
echo hash('sha224', 'a') . "\n";
echo hash('sha224', '012345678901234567890123456789012345678901234567890123456789') . "\n";

/* FIPS-180 Vectors */
echo hash('sha224', 'abc') . "\n";
echo hash('sha224', 'abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq') . "\n";
echo hash('sha224', str_repeat('a', 1000000)) . "\n";
?>
--EXPECT--
d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f
abd37534c7d9a2efb9465de931cd7055ffdb8879563ae98078d6d6d5
ae5c0d27fe120752911c994718296a3bccc77000aac07b8810714932
23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7
75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525
20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67
