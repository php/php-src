--TEST--
Bug #43248 (backward compatibility break in realpath())
--FILE--
<?php
echo realpath(__DIR__ . '/../file/');
?>
--EXPECTF--
%sfile
