--TEST--
bug compatibility: global is used albeit register_globals=0
--SKIPIF--
<?php include('skipif.inc'); 
 if (version_compare(PHP_VERSION,"4.2.3-dev", "<")) die("skip this is for PHP >= 4.2.3");
?>
--INI--
session.use_cookies=0
session.cache_limiter=
register_globals=0
session.bug_compat_42=1
session.bug_compat_warn=1
track_errors=1
log_errors=0
html_errors=0
--FILE--
<?php
error_reporting(E_ALL & ~E_NOTICE);

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
echo $php_errormsg."\n";
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
--EXPECTF--
NULL
Your script possibly relies on a session side-effect which existed until PHP 4.2.3. Please be advised that the session extension does not consider global variables as a source of data, unless register_globals is enabled. You can disable this functionality and this warning by setting session.bug_compat_42 or session.bug_compat_warn to off, respectively.
array(1) {
  ["c"]=>
  float(3.14)
}
NULL
array(1) {
  ["c"]=>
  float(3.14)
}
