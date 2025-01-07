--TEST--
error_get_last() tests
--FILE--
<?php

var_dump(error_get_last());
try {
    var_dump(error_get_last(true));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
var_dump(error_get_last());

$a = $b;

var_dump(error_get_last());

function trigger_fatal_error_with_stacktrace() {
    ini_set('fatal_error_backtraces', true);

    eval("class Foo {}; class Foo {}");
}

register_shutdown_function(function() {
    var_dump(error_get_last());
    echo "Done\n";
});

trigger_fatal_error_with_stacktrace();
?>
--EXPECTF--
NULL
error_get_last() expects exactly 0 arguments, 1 given
NULL

Warning: Undefined variable $b in %s on line %d
array(4) {
  ["type"]=>
  int(2)
  ["message"]=>
  string(21) "Undefined variable $b"
  ["file"]=>
  string(%d) "%s"
  ["line"]=>
  int(11)
}

Fatal error: Cannot redeclare class Foo (previously declared in /Users/enorris/workspace/php-src/ext/standard/tests/general_functions/error_get_last.php(18) : eval()'d code:1) in /Users/enorris/workspace/php-src/ext/standard/tests/general_functions/error_get_last.php(18) : eval()'d code on line 1
Stack trace:
#0 %serror_get_last.php(%d): eval()
#1 %serror_get_last.php(%d): trigger_fatal_error_with_stacktrace()
#2 {main}
array(5) {
  ["type"]=>
  int(64)
  ["message"]=>
  string(%d) "Cannot redeclare class Foo %s"
  ["file"]=>
  string(%d) "%serror_get_last.php(%d) : eval()'d code"
  ["line"]=>
  int(%d)
  ["trace"]=>
  array(2) {
    [0]=>
    array(3) {
      ["file"]=>
      string(%d) "%serror_get_last.php"
      ["line"]=>
      int(%d)
      ["function"]=>
      string(%d) "eval"
    }
    [1]=>
    array(4) {
      ["file"]=>
      string(%d) "%serror_get_last.php"
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
