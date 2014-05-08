--TEST--
filter_var(null,FILTER_VALIDATE_BOOLEAN,FILTER_NULL_ON_FAILURE) returns null

--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>

--FILE--
<?php
var_dump(filter_var(
    null,
    FILTER_VALIDATE_BOOLEAN,
    FILTER_NULL_ON_FAILURE
));

--EXPECTF--
bool(false)

