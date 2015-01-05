--TEST--
stream_context_set_option() function - error : invalid argument
--CREDITS--
Jean-Marc Fontaine <jean-marc.fontaine@alterway.fr>
# Alter Way Contribution Day 2011
--FILE--
<?php
$context = stream_context_create();

// Single option
var_dump(stream_context_set_option($context, 'http'));

// Array of options
var_dump(stream_context_set_option($context, array(), 'foo', 'bar'));
?>
--EXPECTF--
Warning: stream_context_set_option(): called with wrong number or type of parameters; please RTM in %s on line %d
bool(false)

Warning: stream_context_set_option(): called with wrong number or type of parameters; please RTM in %s on line %d
bool(false)
