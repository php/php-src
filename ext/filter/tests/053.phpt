--TEST--
filter_var() - using callback
--SKIPIF--
<?php if (!extension_loaded("filter")) die("skip"); ?>
--FILE--
<?php

function filter_test($str) { return FALSE; }
filter_var('durty/boy', FILTER_CALLBACK, array(
   'options'   => 'filter_test',
));

print "Done\n";

?>
--EXPECT--
Done
