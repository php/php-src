--TEST--
Verify PHP 4.2 compatibility: global is used albeit register_globals=0
--SKIPIF--
<?php include('skipif.inc'); ?>
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);

ini_set("register.globals", 0);
ini_set("session.bug_compat_42", 1);
ini_set("session.bug_compat_warn", 0);

ob_start();
session_id("abtest");

### Phase 1 cleanup
session_start();
session_destroy();

### Phase 2 $HTTP_SESSION_VARS["c"] does not contain any value
session_id("abtest");
session_register("c");
var_dump($c);
unset($c);
$c = 3.14;
session_write_close();
unset($HTTP_SESSION_VARS);
unset($c);

### Phase 3 $HTTP_SESSION_VARS["c"] is set
session_start();
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
NULL
array(1) {
  ["c"]=>
  float(3.14)
}
NULL
array(1) {
  ["c"]=>
  float(3.14)
}
