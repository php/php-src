--TEST--
password_hash(argon2id) handles null bytes correctly
--SKIPIF--
<?php in_array('argon2id', password_algos()) or print 'skip';
--FILE--
<?php

$apwd = password_hash("a", PASSWORD_ARGON2ID);
var_dump($apwd);
var_dump(password_verify("a", $apwd));
var_dump(password_verify("a\0", $apwd));

$apwd = password_hash("a\0", PASSWORD_ARGON2ID);
var_dump($apwd);
var_dump(password_verify("a\0", $apwd));
var_dump(password_verify("a", $apwd));

--EXPECTF--
string(%d) "$argon2id$%s"
bool(true)
bool(false)
string(%d) "$argon2id$%s"
bool(true)
bool(false)
