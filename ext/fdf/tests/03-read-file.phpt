--TEST--
FDF read file
--SKIPIF--
<?php if (!extension_loaded("fdf")) print "skip"; ?>
--FILE--
<?php
$fdf = fdf_open("ext/fdf/tests/simple.fdf");
for ($field = fdf_next_field_name($fdf); $field; $field = fdf_next_field_name($fdf, $field)) {
  echo "$field: ". fdf_get_value($fdf, $field) . "\n";
}
fdf_close($fdf);
?>
--EXPECT--
foo: bar
bar: foo
