--TEST--
OSS-Fuzz #454273637 (UAF with printf optimization and const output)
--FILE--
<?php
printf('%%');
?>
--EXPECT--
%
