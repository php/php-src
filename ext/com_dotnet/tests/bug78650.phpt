--TEST--
Bug #78650 (new COM Crash)
--EXTENSIONS--
com_dotnet
--FILE--
<?php
$fname = __DIR__ . '/bug78650/foo/bar';
mkdir($fname, 0777, true);

$fso = new COM("Scripting.FileSystemObject");
$folder = $fso->GetFolder($fname);
$folder->ParentFolder->Name = 'baz';

print('OK');
?>
--EXPECT--
OK
--CLEAN--
<?php
@rmdir(__DIR__ . '/bug78650/foo/bar');
@rmdir(__DIR__ . '/bug78650/foo');
rmdir(__DIR__ . '/bug78650/baz/bar');
rmdir(__DIR__ . '/bug78650/baz');
rmdir(__DIR__ . '/bug78650');
?>
