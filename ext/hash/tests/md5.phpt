--TEST--
Hash: md5 algorithm
--FILE--
<?php
echo hash('md5', '') . "\n";
echo hash('md5', 'a') . "\n";
echo hash('md5', '012345678901234567890123456789012345678901234567890123456789') . "\n";
echo hash('md5', str_repeat('a', 1000000)) . "\n";
?>
--EXPECT--
d41d8cd98f00b204e9800998ecf8427e
0cc175b9c0f1b6a831c399e269772661
1ced811af47ead374872fcca9d73dd71
7707d6ae4e027c70eea2a935c2296f21
