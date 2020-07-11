--TEST--
Hash: sha384 algorithm
--FILE--
<?php
echo hash('sha384', '') . "\n";
echo hash('sha384', 'a') . "\n";
echo hash('sha384', '012345678901234567890123456789012345678901234567890123456789') . "\n";

/* FIPS-180 Vectors */
echo hash('sha384', 'abc') . "\n";
echo hash('sha384', 'abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu') . "\n";
echo hash('sha384', str_repeat('a', 1000000)) . "\n";
--EXPECT--
38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b
54a59b9f22b0b80880d8427e548b7c23abd873486e1f035dce9cd697e85175033caa88e6d57bc35efae0b5afd3145f31
ce6bebce38aad0fd35805b50f77f3e1814d46df8e930356ec905a5d7b94bfa615fce4c3b6caf50eb4a7f1a1164887470
cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed8086072ba1e7cc2358baeca134c825a7
09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712fcc7c71a557e2db966c3e9fa91746039
9d0e1809716474cb086e834e310a4a1ced149e9c00f248527972cec5704c2a5b07b8b3dc38ecc4ebae97ddd87f3d8985
