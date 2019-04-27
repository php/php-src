--TEST--
zip_read() function
--SKIPIF--
<?php
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$zip = zip_open(__DIR__."/test_procedural.zip");
if (!is_resource($zip)) die("Failure");
$entries = 0;
while ($entry = zip_read($zip)) {
  $entries++;
}
zip_close($zip);
echo "$entries entries";

?>
--EXPECT--
4 entries
