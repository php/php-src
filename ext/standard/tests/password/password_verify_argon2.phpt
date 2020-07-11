--TEST--
Test normal operation of password_verify() with Argon2i and Argon2id
--SKIPIF--
<?php
if (!defined('PASSWORD_ARGON2I')) die('skip password_verify not built with Argon2');
if (!defined('PASSWORD_ARGON2ID')) die('skip password_hash not built with Argon2');
?>
--FILE--
<?php

var_dump(password_verify('test', '$argon2i$v=19$m=65536,t=3,p=1$OEVjWWs2Z3YvWlNZQ0ZmNw$JKin7ahjmh8JYvMyFcXri0Ss/Uvd3uYpD7MG6C/5Cy0'));

var_dump(password_verify('argon2', '$argon2i$v=19$m=65536,t=3,p=1$OEVjWWs2Z3YvWlNZQ0ZmNw$JKin7ahjmh8JYvMyFcXri0Ss/Uvd3uYpD7MG6C/5Cy0'));

var_dump(password_verify('test', '$argon2id$v=19$m=1024,t=2,p=2$WS90MHJhd3AwSC5xTDJpZg$8tn2DaIJR2/UX4Cjcy2t3EZaLDL/qh+NbLQAOvTmdAg'));
var_dump(password_verify('argon2id', '$argon2id$v=19$m=1024,t=2,p=2$WS90MHJhd3AwSC5xTDJpZg$8tn2DaIJR2/UX4Cjcy2t3EZaLDL/qh+NbLQAOvTmdAg'));
echo "OK!";
?>
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
OK!
