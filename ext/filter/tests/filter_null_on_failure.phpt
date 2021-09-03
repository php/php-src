--TEST--
FILTER_NULL_ON_FAILURE will give NULL on filters
--EXTENSIONS--
filter
--FILE--
<?php
var_dump(filter_var("invalid", FILTER_VALIDATE_BOOL, FILTER_NULL_ON_FAILURE));
var_dump(filter_var("invalid", FILTER_VALIDATE_BOOLEAN, FILTER_NULL_ON_FAILURE));
var_dump(filter_var("invalid", FILTER_VALIDATE_INT, FILTER_NULL_ON_FAILURE));
var_dump(filter_var("invalid", FILTER_VALIDATE_FLOAT, FILTER_NULL_ON_FAILURE));
var_dump(filter_var(".invalid", FILTER_VALIDATE_DOMAIN, FILTER_NULL_ON_FAILURE));
var_dump(filter_var("invalid", FILTER_VALIDATE_EMAIL, FILTER_NULL_ON_FAILURE));
var_dump(filter_var("invalid", FILTER_VALIDATE_IP, FILTER_NULL_ON_FAILURE));
var_dump(filter_var("invalid", FILTER_VALIDATE_MAC, FILTER_NULL_ON_FAILURE));
var_dump(filter_var("invalid", FILTER_VALIDATE_REGEXP, [
    'flags' => FILTER_NULL_ON_FAILURE,
    'options' => [
        'regexp' => '/^valid$/'
    ]
]));
var_dump(filter_var("invalid", FILTER_VALIDATE_URL, FILTER_NULL_ON_FAILURE));
?>
--EXPECT--
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
NULL
