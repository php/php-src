--TEST--
Bug #68532: convert.base64-encode omits padding bytes
--FILE--
<?php
$testString = 'test';
$stream = fopen('php://memory','r+');
fwrite($stream, $testString);
rewind($stream);
$filter = stream_filter_append($stream, 'convert.base64-encode');
echo "memoryStream = " . stream_get_contents($stream).PHP_EOL;


$fileStream = fopen(__DIR__ . '/base64test.txt','w+');
fwrite($fileStream , $testString);
rewind($fileStream );
$filter = stream_filter_append($fileStream , 'convert.base64-encode');
echo "fileStream = " . stream_get_contents($fileStream ).PHP_EOL;
?>
--CLEAN--
<?php
unlink(__DIR__ . '/base64test.txt');
?>
--EXPECT--
memoryStream = dGVzdA==
fileStream = dGVzdA==
