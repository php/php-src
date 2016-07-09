--TEST--
Test normal operation of password_verify)
--FILE--
<?php
//-=-=-=-

var_dump(password_verify(123, 123));

var_dump(password_verify("foo", '$2a$07$usesomesillystringforsalt$'));

var_dump(password_verify('rasmusler', '$2a$07$usesomesillystringfore2uDLvp1Ii2e./U9C8sBjqp8I90dH6hi'));

var_dump(password_verify('rasmuslerdorf', '$2a$07$usesomesillystringfore2uDLvp1Ii2e./U9C8sBjqp8I90dH6hi'));

var_dump(password_verify("foo", null));

var_dump(password_verify("rasmuslerdorf", "rl.3StKT.4T8M"));

var_dump(password_verify("foo", "$1"));

var_dump(password_verify('test', '$argon2d$v=19$m=32768,t=2,p=1$YWpxd0VYRW9MLmp6VjFPZw$pWV5IsbBfjEK5c0bHzvAo0FsDNHUyM4p6j8vf2cxzb8'));

var_dump(password_verify('argon2', '$argon2d$v=19$m=32768,t=2,p=1$YWpxd0VYRW9MLmp6VjFPZw$pWV5IsbBfjEK5c0bHzvAo0FsDNHUyM4p6j8vf2cxzb8'));

var_dump(password_verify('test', '$argon2i$v=19$m=65536,t=3,p=1$OEVjWWs2Z3YvWlNZQ0ZmNw$JKin7ahjmh8JYvMyFcXri0Ss/Uvd3uYpD7MG6C/5Cy0'));

var_dump(password_verify('argon2', '$argon2i$v=19$m=65536,t=3,p=1$OEVjWWs2Z3YvWlNZQ0ZmNw$JKin7ahjmh8JYvMyFcXri0Ss/Uvd3uYpD7MG6C/5Cy0'));
echo "OK!";
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
OK!