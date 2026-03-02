--TEST--
error_get_last() w/ fatal error
--INI--
fatal_error_backtraces=On
--FILE--
<?php

function trigger_fatal_error_with_stacktrace() {
    eval("class Foo {}; class Foo {}");
}

register_shutdown_function(function() {
    var_dump(error_get_last());
    echo "Done\n";
});

trigger_fatal_error_with_stacktrace();
?>
--EXPECTF--
Fatal error: Cannot redeclare class Foo (%s) in %s on line %d
Stack trace:
#0 %serror_get_last_002.php(%d): eval()
#1 %serror_get_last_002.php(%d): trigger_fatal_error_with_stacktrace()
#2 {main}
array(5) {
  ["type"]=>
  int(64)
  ["message"]=>
  string(%d) "Cannot redeclare class Foo %s"
  ["file"]=>
  string(%d) "%serror_get_last_002.php(%d) : eval()'d code"
  ["line"]=>
  int(%d)
  ["trace"]=>
  array(2) {
    [0]=>
    array(3) {
      ["file"]=>
      string(%d) "%serror_get_last_002.php"
      ["line"]=>
      int(%d)
      ["function"]=>
      string(%d) "eval"
    }
    [1]=>
    array(4) {
      ["file"]=>
      string(%d) "%serror_get_last_002.php"
      ["line"]=>
      int(%d)
      ["function"]=>
      string(%d) "trigger_fatal_error_with_stacktrace"
      ["args"]=>
      array(0) {
      }
    }
  }
}
Done
