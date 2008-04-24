--TEST--
Bug#44806 (rename() function is not portable to Windows)
--FILE--
<?php

file_put_contents("file1.txt", "this is file 1");
file_put_contents("file2.txt", "this is file 2");

rename("file1.txt", "file2.txt");

echo "reading file 2: ";
readfile("file2.txt");
?>
--EXPECT--
reading file 2: this is file 1
