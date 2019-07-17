--TEST--
stream_resolve_include_path(string path)
--FILE--
<?php
$include_path = __DIR__ . '/test_path';
$include_path_nested = $include_path . '/nested';

$include_path_file = $include_path . DIRECTORY_SEPARATOR . 'file';
$include_path_nested_file = $include_path_nested . DIRECTORY_SEPARATOR . 'file';

mkdir($include_path);
mkdir($include_path_nested);

file_put_contents($include_path_file, 'include_path');
file_put_contents($include_path_nested_file, 'include_path');

set_include_path($include_path . PATH_SEPARATOR . $include_path_nested);
var_dump(stream_resolve_include_path('file-does-not-exist'));

set_include_path($include_path . PATH_SEPARATOR . $include_path_nested);
var_dump(stream_resolve_include_path('file'));
set_include_path($include_path_nested . PATH_SEPARATOR . $include_path);
var_dump(stream_resolve_include_path('file'));

?>
--CLEAN--
<?php
$include_path = __DIR__ . '/test_path';
$include_path_nested = $include_path . '/nested';
$include_path_file = $include_path . DIRECTORY_SEPARATOR . 'file';
$include_path_nested_file = $include_path_nested . DIRECTORY_SEPARATOR . 'file';

unlink($include_path_nested_file);
rmdir($include_path_nested);
unlink($include_path_file);
rmdir($include_path);
?>
--EXPECTF--
bool(false)
string(%d) "%stest_path%sfile"
string(%d) "%stest_path%snested%sfile"
