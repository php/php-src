--TEST--
Bug #54060 (Memory leak in openssl_encrypt)
--EXTENSIONS--
openssl
--FILE--
<?php

$data = "jfdslkjvflsdkjvlkfjvlkjfvlkdm,4w 043920r 9234r 32904r 09243
r7-89437 r892374 r894372 r894 7289r7 f  frwerfh i iurf iuryw uyrfouiwy ruy
972439 8478942 yrhfjkdhls";
$pass = "r23498rui324hjbnkj";

openssl_encrypt($data, 'des3', $pass, 0, '1qazxsw2');
echo "Done";
?>
--EXPECT--
Done
