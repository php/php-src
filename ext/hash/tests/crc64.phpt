--TEST--
Hash: CRC64 algorithms
--FILE--
<?php
echo "crc64-nvme\n";
echo hash('crc64-nvme', ''), "\n";
echo hash('crc64-nvme', 'a'), "\n";
echo hash('crc64-nvme', 'abc'), "\n";
echo hash('crc64-nvme', 'message digest'), "\n";
echo hash('crc64-nvme', 'abcdefghijklmnopqrstuvwxyz'), "\n";
echo hash('crc64-nvme', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'), "\n";
echo hash('crc64-nvme', '12345678901234567890123456789012345678901234567890123456789012345678901234567890'), "\n";
echo hash('crc64-nvme', '123456789'), "\n";

echo "crc64-ecma-182\n";
echo hash('crc64-ecma-182', ''), "\n";
echo hash('crc64-ecma-182', 'a'), "\n";
echo hash('crc64-ecma-182', 'abc'), "\n";
echo hash('crc64-ecma-182', 'message digest'), "\n";
echo hash('crc64-ecma-182', 'abcdefghijklmnopqrstuvwxyz'), "\n";
echo hash('crc64-ecma-182', 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789'), "\n";
echo hash('crc64-ecma-182', '12345678901234567890123456789012345678901234567890123456789012345678901234567890'), "\n";
echo hash('crc64-ecma-182', '123456789'), "\n";

?>
--EXPECT--
crc64-nvme
0000000000000000
8c2f8445b4cbfc3c
05e5cabb3fc1faeb
dc4a92ab26fba23f
1a110d6a11fe63a8
8b8f30cfc6f16409
24098f6874d98832
ae8b14860a799888
crc64-ecma-182
0000000000000000
548f120162451c62
66501a349a0e0855
c04d61278997ba5e
97a2566b552fcc4e
5ca18585b92c58b9
be4c46263774953c
6c40df5f0b497347