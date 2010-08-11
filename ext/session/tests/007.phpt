--TEST--
bug compatibility: unset($c) with enabled register_globals
--SKIPIF--
<?php include('skipif.inc'); if (PHP_VERSION_ID < 503099) { echo 'skip requires register_globals'; }  ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.serialize_handler=php
session.save_handler=files
precision=14
--FILE--
<?php
error_reporting(E_ALL);

session_id("abtest");

### Phase 1 cleanup
session_start();
session_destroy();

### Phase 2 $_SESSION["c"] does not contain any value
session_id("abtest");
session_register("c");
unset($c);
$c = 3.14;
session_write_close();
unset($_SESSION);
unset($c);

### Phase 3 $_SESSION["c"] is set
session_start();
var_dump($c);
var_dump($_SESSION);
unset($c);
$c = 2.78;

session_write_close();
unset($_SESSION);
unset($c);

### Phase 4 final

session_start();
var_dump($c);
var_dump($_SESSION);

session_destroy();
?>
--EXPECTF--
Deprecated: Directive 'register_globals' is deprecated in PHP 5.3 and greater in Unknown on line 0

Deprecated: Function session_register() is deprecated in %s on line %d
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
