--TEST--
filter_var() - using callback
--EXTENSIONS--
filter
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
