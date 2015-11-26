--TEST--
Bug #67167: object with VALIDATE_BOOLEAN and NULL_ON_FAILURE

--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>

--FILE--
<?php
var_dump(filter_var(
    new \StdClass(),
    FILTER_VALIDATE_BOOLEAN,
    FILTER_NULL_ON_FAILURE
));

--EXPECTF--
NULL
