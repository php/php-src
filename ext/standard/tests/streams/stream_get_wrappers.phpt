--TEST--
array stream_get_wrappers ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
  die('SKIP php version so lower.');
}
?>
--FILE--
<?php
print((is_array(stream_get_wrappers())) ? ("yes") : ("Test 'array stream_get_wrappers ( void );' has failed"));
echo "\n";
class Foo { }
stream_wrapper_register("foo", "Foo");
var_dump(in_array("foo", stream_get_wrappers()));
?>
--EXPECT--
yes
bool(true)
