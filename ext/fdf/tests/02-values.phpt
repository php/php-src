--TEST--
FDF open/save and set/get values
--SKIPIF--
<?php if (!extension_loaded("fdf")) print "skip"; ?>
--FILE--
<?php
$f1 = fdf_create();
fdf_set_value($f1, "foo", "bar");
fdf_set_value($f1, "bar", "foo");
$f2 = fdf_open_string(fdf_save_string($f1));
fdf_close($f1);
for ($field = fdf_next_field_name($f2); $field; $field = fdf_next_field_name($f2, $field)) {
  echo "$field: ". fdf_get_value($f2, $field) . "\n";
}
fdf_close($f2);
?>
--EXPECT--
foo: bar
bar: foo
