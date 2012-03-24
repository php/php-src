--TEST--
Bug #60738 Allow 'set_error_handler' to handle NULL
--FILE--
<?php

set_error_handler(function() { echo 'Intercepted error!', "\n"; });

trigger_error('Error!');

set_error_handler(null);

trigger_error('Error!');
?>
--EXPECTF--
Intercepted error!

Notice: Error! in %s on line %d
