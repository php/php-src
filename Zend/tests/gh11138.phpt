--TEST--
move_uploaded_file() emits open_basedir warning for source file
--POST_RAW--
Content-type: multipart/form-data, boundary=AaB03x

--AaB03x
content-disposition: form-data; name="file"; filename="file.txt"
Content-Type: text/plain

foo
--AaB03x--
--FILE--
<?php

ini_set('open_basedir', __DIR__);

$destination = __DIR__ . '/gh11138.tmp';
var_dump(move_uploaded_file($_FILES['file']['tmp_name'], $destination));
echo file_get_contents($destination), "\n";

?>
--CLEAN--
<?php
@unlink(__DIR__ . '/gh11138.tmp');
?>
--EXPECT--
bool(true)
foo
