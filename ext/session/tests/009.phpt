--TEST--
unset($_SESSION["name"]); test
--SKIPIF--
<?php include('skipif.inc'); ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=files
--FILE--
<?php
error_reporting(E_ALL);

session_id("abtest");

### Phase 1 cleanup
session_start();
session_destroy();

### Phase 2 $_SESSION["c"] does not contain any value
session_id("abtest");
session_start();
var_dump($_SESSION);
$_SESSION["name"] = "foo";
var_dump($_SESSION);
session_write_close();

### Phase 3 $_SESSION["c"] is set
session_start();
var_dump($_SESSION);
unset($_SESSION["name"]);
var_dump($_SESSION);
session_write_close();

### Phase 4 final

session_start();
var_dump($_SESSION);
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

