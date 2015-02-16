--TEST--
test compiled reason
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php
$next = 1;
$data = array(
    "key" => "X-HTTP ",
    "value" => "testing"
);

class HeaderMalfunctionException extends AssertionException {}

assert (preg_match("~^([a-zA-Z0-9-]+)$~", $data["key"]), new HeaderMalfunctionException("malformed key found at {$next} \"{$data["key"]}\""));
?>
--EXPECTF--
Fatal error: Uncaught exception 'HeaderMalfunctionException' with message 'malformed key found at 1 "X-HTTP "' in %sexpect_007.php:10
Stack trace:
#0 {main}
  thrown in %sexpect_007.php on line 10
