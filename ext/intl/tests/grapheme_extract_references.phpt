--TEST--
grapheme_extract() references handling
--EXTENSIONS--
intl
--FILE--
<?php
class Test {
    public string $prop = "a";
}
$test = new Test;
$next =& $test->prop;
grapheme_extract("test", 4, next: $next);
var_dump($test);
?>
--EXPECT--
object(Test)#1 (1) {
  ["prop"]=>
  &string(1) "4"
}
