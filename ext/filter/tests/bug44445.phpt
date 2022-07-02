--TEST--
Bug #44445 (email validator does not handle domains starting/ending with a -)
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var("foo@-foo.com",FILTER_VALIDATE_EMAIL));
var_dump(filter_var("foo@foo-.com",FILTER_VALIDATE_EMAIL));
?>
--EXPECT--
bool(false)
bool(false)
