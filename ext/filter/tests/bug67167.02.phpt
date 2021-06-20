--TEST--
Bug #67167: filter_var(null,FILTER_VALIDATE_BOOLEAN,FILTER_NULL_ON_FAILURE) returns null
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var(
    null,
    FILTER_VALIDATE_BOOLEAN,
    FILTER_NULL_ON_FAILURE
));
?>
--XFAIL--
Requires php_zval_filter to not use convert_to_string for all filters.
--EXPECT--
NULL
