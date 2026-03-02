--TEST--
Bug GH-9883 (SplFileObject::__toString() reads next line)
--FILE--
<?php
$file_stream = new SplFileObject(__FILE__, 'rb');

echo $file_stream; // line 4
echo $file_stream; // line 5
echo $file_stream; // line 6
echo $file_stream; // line 7
echo $file_stream; // line 8
echo $file_stream; // line 9
?>
--EXPECT--
<?php
<?php
<?php
<?php
<?php
<?php
