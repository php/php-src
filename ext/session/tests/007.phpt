--TEST--
Verify PHP 4.2 compatibility: unset($c) with enabled register_globals
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
register_globals=1
session.bug_compat_42=1
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);

ob_start();
session_id("abtest");

### Phase 1 cleanup
session_start();
session_destroy();

### Phase 2 $HTTP_SESSION_VARS["c"] does not contain any value
session_id("abtest");
session_register("c");
unset($c);
$c = 3.14;
session_write_close();
unset($HTTP_SESSION_VARS);
unset($c);

### Phase 3 $HTTP_SESSION_VARS["c"] is set
session_start();
var_dump($c);
var_dump($HTTP_SESSION_VARS);
unset($c);
$c = 2.78;

session_write_close();
unset($HTTP_SESSION_VARS);
unset($c);

### Phase 4 final

session_start();
var_dump($c);
var_dump($HTTP_SESSION_VARS);

session_destroy();
?>
--EXPECT--
float(3.14)
array(1) {
  ["c"]=>
  &float(3.14)
}
float(3.14)
array(1) {
  ["c"]=>
  &float(3.14)
}
