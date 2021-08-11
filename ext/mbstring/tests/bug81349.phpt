--TEST--
Bug #81349: mb_detect_encoding misdetcts ASCII in some cases
--EXTENSIONS--
mbstring
--FILE--
<?php

echo(mb_detect_encoding("\xe4,a", ['ASCII', 'UTF-8', 'ISO-8859-1'])."\n");
echo(mb_detect_encoding("\xe4 a", ['ASCII', 'UTF-8', 'ISO-8859-1'])."\n");

?>
--EXPECT--
ISO-8859-1
ISO-8859-1
