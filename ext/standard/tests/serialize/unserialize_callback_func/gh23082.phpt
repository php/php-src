--TEST--
Bug GH-23082: unserialize_callback_func can no longer be reset to its empty default at runtime
--FILE--
<?php

function my_callback($name) { echo "callback fired for $name\n"; }

// Save the current value and install our own, the usual save/restore idiom.
$prev = ini_set('unserialize_callback_func', 'my_callback');
var_dump($prev);

// Restore it. $prev is "" here, since that is the default value.
var_dump(ini_set('unserialize_callback_func', $prev));
var_dump(ini_get('unserialize_callback_func'));

// The callback is still installed and fires for unrelated code.
$o = unserialize('O:20:"SomeNotExistingClass":0:{}');

?>
--EXPECT--
string(0) ""
string(11) "my_callback"
string(0) ""
