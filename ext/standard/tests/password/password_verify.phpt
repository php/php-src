--TEST--
Test normal operation of password_verify)
--FILE--
<?php
//-=-=-=-

var_dump(password_verify(123, 123));

var_dump(password_verify("foo", '$2a$07$usesomesillystringforsalt$'));

var_dump(password_verify('rasmusler', '$2a$07$usesomesillystringfore2uDLvp1Ii2e./U9C8sBjqp8I90dH6hi'));

var_dump(password_verify('rasmuslerdorf', '$2a$07$usesomesillystringfore2uDLvp1Ii2e./U9C8sBjqp8I90dH6hi'));
echo "OK!";
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(true)
OK!
