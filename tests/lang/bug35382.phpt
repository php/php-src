--TEST--
Bug #35382 (Comment in end of file produces fatal error)
--FILEEOF--
<?php
eval("echo 'Hello'; // comment");
echo " World";
//last line comment
--EXPECT--
Hello World
