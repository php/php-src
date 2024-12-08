--TEST--
GH-17067 (glob:// wrapper doesn't cater to CWD for ZTS builds)
--FILE--
<?php
$dir = __DIR__ . "/gh17067";
mkdir($dir);
touch("$dir/foo");

chdir($dir);
var_dump(scandir("glob://*"));
?>
--CLEAN--
<?php
$dir = __DIR__ . "/gh17067";
@unlink("$dir/foo");
@rmdir($dir);
?>
--EXPECT--
array(1) {
  [0]=>
  string(3) "foo"
}
