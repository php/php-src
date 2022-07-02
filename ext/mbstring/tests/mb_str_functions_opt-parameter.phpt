--TEST--
Optional long parameter might be null (deprecated)
--EXTENSIONS--
mbstring
--FILE--
<?php
echo mb_strpos('abb', 'b', null, 'UTF-8') . "\n";
echo mb_strrpos('abb', 'b', null, 'UTF-8') . "\n";
echo mb_stripos('abb', 'B', null, 'UTF-8') . "\n";
echo mb_strripos('abb', 'B', null, 'UTF-8') . "\n";
echo mb_strstr('foobarbaz', 'ba', null, 'UTF-8') . "\n";
echo mb_strrchr('foobarbaz', 'ba', null, 'UTF-8') . "\n";
echo mb_stristr('foobarbaz', 'BA', null, 'UTF-8') . "\n";
echo mb_strrichr('foobarbaz', 'BA', null, 'UTF-8') . "\n";
echo mb_substr('foobarbaz', 6, null, 'UTF-8') . "\n";
echo mb_strcut('foobarbaz', 6, null, 'UTF-8') . "\n";
echo mb_strimwidth('foobar', 0, 3, null, 'UTF-8') . "\n";
?>
--EXPECTF--
Deprecated: mb_strpos(): Passing null to parameter #3 ($offset) of type int is deprecated in %s on line %d
1

Deprecated: mb_strrpos(): Passing null to parameter #3 ($offset) of type int is deprecated in %s on line %d
2

Deprecated: mb_stripos(): Passing null to parameter #3 ($offset) of type int is deprecated in %s on line %d
1

Deprecated: mb_strripos(): Passing null to parameter #3 ($offset) of type int is deprecated in %s on line %d
2

Deprecated: mb_strstr(): Passing null to parameter #3 ($before_needle) of type bool is deprecated in %s on line %d
barbaz

Deprecated: mb_strrchr(): Passing null to parameter #3 ($before_needle) of type bool is deprecated in %s on line %d
baz

Deprecated: mb_stristr(): Passing null to parameter #3 ($before_needle) of type bool is deprecated in %s on line %d
barbaz

Deprecated: mb_strrichr(): Passing null to parameter #3 ($before_needle) of type bool is deprecated in %s on line %d
baz
baz
baz

Deprecated: mb_strimwidth(): Passing null to parameter #4 ($trim_marker) of type string is deprecated in %s on line %d
foo
