--TEST--
json_decode() tests
--SKIPIF--
<?php if (!extension_loaded("json")) print "skip"; ?>
--FILE--
<?php

var_dump(json_decode());
var_dump(json_decode(""));
var_dump(json_decode("", 1));
var_dump(json_decode("", 0));
var_dump(json_decode(".", 1));
var_dump(json_decode(".", 0));
var_dump(json_decode("<?>"));
var_dump(json_decode(";"));
var_dump(json_decode("руссиш"));
var_dump(json_decode("blah"));
var_dump(json_decode(NULL));
var_dump(json_decode('{ "test": { "foo": "bar" } }'));
var_dump(json_decode('{ "test": { "foo": "" } }'));
var_dump(json_decode('{ "": { "foo": "" } }'));
var_dump(json_decode('{ "": { "": "" } }'));
var_dump(json_decode('{ "": { "": "" }'));
var_dump(json_decode('{ "": "": "" } }'));

echo "Done\n";
?>
--EXPECTF--
Warning: json_decode() expects at least 1 parameter, 0 given in %s on line %d
NULL
NULL
NULL
NULL
unicode(1) "."
unicode(1) "."
unicode(3) "<?>"
unicode(1) ";"
unicode(6) "руссиш"
unicode(4) "blah"
NULL
object(stdClass)#%d (1) {
  [u"test"]=>
  object(stdClass)#%d (1) {
    [u"foo"]=>
    unicode(3) "bar"
  }
}
object(stdClass)#%d (1) {
  [u"test"]=>
  object(stdClass)#%d (1) {
    [u"foo"]=>
    unicode(0) ""
  }
}
object(stdClass)#%d (1) {
  [u"_empty_"]=>
  object(stdClass)#%d (1) {
    [u"foo"]=>
    unicode(0) ""
  }
}
object(stdClass)#%d (1) {
  [u"_empty_"]=>
  object(stdClass)#%d (1) {
    [u"_empty_"]=>
    unicode(0) ""
  }
}
unicode(16) "{ "": { "": "" }"
unicode(16) "{ "": "": "" } }"
Done
