--TEST--
Bug #81424 (PCRE2 10.35 JIT performance regression)
--DESCRIPTION--
We're testing against the functional regression which has been introduced by
fixing the performance regression.
--FILE--
<?php
var_dump(
    preg_match('/(?P<size>\d+)m|M/', "4M", $m),
    $m
);
?>
--EXPECT--
int(1)
array(1) {
  [0]=>
  string(1) "M"
}
