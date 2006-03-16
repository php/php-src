--TEST--
bug compatibility: unset($c) with enabled register_globals
--SKIPIF--
<?php die('skip');/* RG removed */ ?>
--INI--
session.use_cookies=0
session.cache_limiter=
session.bug_compat_42=1
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
$_SESSION['c'] = $c;

unset($c);
$c = 3.14;
session_write_close();
unset($_SESSION);
unset($c);

### Phase 3 $_SESSION["c"] is set
session_start();
$c = $_SESSION['c'];
var_dump($c);
var_dump($_SESSION);
unset($c);
$c = 2.78;

session_write_close();
unset($_SESSION);
unset($c);

### Phase 4 final

session_start();
$c = $_SESSION['c'];
var_dump($c);
var_dump($_SESSION);

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
