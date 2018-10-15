--TEST--
Test normal operation of password_verify() with argon2
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_verify not built with Argon2');
?>
--FILE--
<?php

var_dump(password_verify('test', '$argon2i$v=19$m=65536,t=3,p=1$OEVjWWs2Z3YvWlNZQ0ZmNw$JKin7ahjmh8JYvMyFcXri0Ss/Uvd3uYpD7MG6C/5Cy0'));

var_dump(password_verify('argon2', '$argon2i$v=19$m=65536,t=3,p=1$OEVjWWs2Z3YvWlNZQ0ZmNw$JKin7ahjmh8JYvMyFcXri0Ss/Uvd3uYpD7MG6C/5Cy0'));
echo "OK!";
?>
--EXPECT--
bool(true)
bool(false)
OK!
