--TEST--
ob_start(): Ensure that user supplied handler type and status flags are erased
--FILE--
<?php
define('PHP_OUTOPUT_HANDLER_TYPE_INTERNAL', 0);

ob_start(
    null,
    0,
    PHP_OUTPUT_HANDLER_STDFLAGS |
    PHP_OUTOPUT_HANDLER_TYPE_INTERNAL |
    PHP_OUTPUT_HANDLER_STARTED |
    PHP_OUTPUT_HANDLER_DISABLED |
    PHP_OUTPUT_HANDLER_PROCESSED
);

$bitmask = ob_get_status()['flags'];

var_dump($bitmask & PHP_OUTPUT_HANDLER_STDFLAGS);
var_dump($bitmask & PHP_OUTOPUT_HANDLER_TYPE_INTERNAL);
var_dump($bitmask & PHP_OUTPUT_HANDLER_STARTED);
var_dump($bitmask & PHP_OUTPUT_HANDLER_DISABLED);
var_dump($bitmask & PHP_OUTPUT_HANDLER_PROCESSED);
?>
--EXPECT--
int(112)
int(0)
int(0)
int(0)
int(0)
