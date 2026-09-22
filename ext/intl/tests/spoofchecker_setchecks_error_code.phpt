--TEST--
Spoofchecker::setChecks() records the ICU error code
--EXTENSIONS--
intl
--SKIPIF--
<?php if (!class_exists("Spoofchecker")) print 'skip'; ?>
--FILE--
<?php

$s = new Spoofchecker();
$s->setChecks(1 << 20);
var_dump(intl_get_error_code(), intl_get_error_message());

?>
--EXPECTF--
Warning: Spoofchecker::setChecks(): (1) U_ILLEGAL_ARGUMENT_ERROR in %s on line %d
int(1)
string(24) "U_ILLEGAL_ARGUMENT_ERROR"
