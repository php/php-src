--TEST--
Bug #52929 (Segfault in filter_var with FILTER_VALIDATE_EMAIL with large amount of data)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var('valid@email.address', FILTER_VALIDATE_EMAIL));

// Beyond the allowable limit for an e-mail address.
var_dump(filter_var('xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx@yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy.zz', FILTER_VALIDATE_EMAIL));

// An invalid address likely to crash PHP due to stack exhaustion if it goes to
// the validation regex.
var_dump(filter_var(str_repeat('x', 8000), FILTER_VALIDATE_EMAIL));
?>
--EXPECT--
string(19) "valid@email.address"
bool(false)
bool(false)
