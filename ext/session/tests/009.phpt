--TEST--
unset($_SESSION["name"]); should work with register_globals=off
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
register_long_arrays=1
session.use_cookies=0
session.cache_limiter=
register_globals=0
session.bug_compat_42=1
session.bug_compat_warn=0
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

session_id("abtest");

### Phase 1 cleanup
session_start();
session_destroy();

### Phase 2 $HTTP_SESSION_VARS["c"] does not contain any value
session_id("abtest");
session_start();
var_dump($HTTP_SESSION_VARS);
$HTTP_SESSION_VARS["name"] = "foo";
var_dump($HTTP_SESSION_VARS);
session_write_close();

### Phase 3 $HTTP_SESSION_VARS["c"] is set
session_start();
var_dump($HTTP_SESSION_VARS);
unset($HTTP_SESSION_VARS["name"]);
var_dump($HTTP_SESSION_VARS);
session_write_close();

### Phase 4 final

session_start();
var_dump($HTTP_SESSION_VARS);
session_destroy();
?>
--EXPECT--
array(0) {
}
array(1) {
  ["name"]=>
  string(3) "foo"
}
array(1) {
  ["name"]=>
  string(3) "foo"
}
array(0) {
}
array(0) {
}
