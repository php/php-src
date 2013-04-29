--TEST--
Bug #43248 (backward compatibility break in realpath())
--FILE--
<?php
echo realpath(dirname(__FILE__) . '/../file/');
?>
--EXPECTF--
%sfile
