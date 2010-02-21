--TEST--
Bug #51059 crypt() segfaults on certain salts
--FILE--
<?php

if (crypt('a', '_') === FALSE) echo 'OK';
else echo 'Not OK';

?>
--EXPECT--
OK
