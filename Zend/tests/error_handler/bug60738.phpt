--TEST--
Bug #60738 Allow 'set_error_handler' to handle NULL
--FILE--
<?php

var_dump(set_error_handler(
    function() { echo 'Intercepted error!', "\n"; }
));

trigger_error('Error!');

var_dump(set_error_handler(null));

trigger_error('Error!');
?>
--EXPECTF--
NULL
Intercepted error!
object(Closure)#%d (3) {
  ["name"]=>
  string(%d) "{closure:%s:%d}"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(%d)
}

Notice: Error! in %s on line %d
