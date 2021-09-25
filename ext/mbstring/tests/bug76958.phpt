--TEST--
Bug #76958: Broken UTF7-IMAP conversion
--EXTENSIONS--
mbstring
--FILE--
<?php

$str = '&BCAEMARBBEEESwQ7BDoEOA-';
echo mb_convert_encoding($str, 'UTF-8', 'UTF7-IMAP') . "\n";
$str = 'Рассылки';
echo mb_convert_encoding($str, 'UTF7-IMAP', 'UTF-8') . "\n";

?>
--EXPECT--
Рассылки
&BCAEMARBBEEESwQ7BDoEOA-
