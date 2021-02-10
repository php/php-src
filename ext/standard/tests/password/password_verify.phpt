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

echo "OK!";
?>
--EXPECTF--
bool(false)
bool(false)
bool(false)
bool(true)

Deprecated: password_verify(): Passing null to parameter #2 ($hash) of type string is deprecated in %s on line %d
bool(false)
bool(true)
bool(false)
OK!
