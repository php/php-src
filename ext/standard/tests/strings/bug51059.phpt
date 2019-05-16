--TEST--
Bug #51059 crypt() segfaults on certain salts
--FILE--
<?php
$res = crypt('a', '_');
if ($res === '*0' || $res === '*1') echo 'OK';
else echo 'Not OK';

?>
--EXPECT--
OK
