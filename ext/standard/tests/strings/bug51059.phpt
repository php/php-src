--TEST--
Bug #51059 crypt() segfaults on certain salts
--XFAIL--
Needs a patch from Pierre
--FILE--
<?php

if (crypt('a', '_') === FALSE) echo 'OK';
else echo 'Not OK';

?>
--EXPECT--
OK
