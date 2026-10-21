--TEST--
OOM inside an internal output handler must not free the running handler
--EXTENSIONS--
mbstring
--INI--
memory_limit=64M
output_encoding=HTML-ENTITIES
mbstring.encoding_translation=1
--FILE--
<?php
/* mb_output_handler() is an internal function, so a delayed OOM raised while it
 * runs is only noticed after it returns, in zend_call_function(). The fatal
 * error raised there must not discard the output buffers: doing so frees every
 * handler on the stack, including the one php_output_stack_pop() is still
 * holding. */
ob_start('mb_output_handler');
echo str_repeat("\u{00e9}", 200000);
ini_set('memory_limit', memory_get_usage(true) + 1024 * 1024);
?>
--EXPECT--
