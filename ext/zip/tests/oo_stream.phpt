--TEST--
getStream
--EXTENSIONS--
zip
--FILE--
<?php
$dirname = __DIR__ . '/';
$file = $dirname . 'test_with_comment.zip';
include $dirname . 'utils.inc';
$zip = new ZipArchive;
if (!$zip->open($file)) {
    exit('failed');
}
$fp = $zip->getStream('foo');

var_dump($fp);
if(!$fp) exit("\n");
$contents = '';
while (!feof($fp)) {
    $contents .= fread($fp, 255);
}

fclose($fp);
$zip->close();
var_dump($contents);


$fp = fopen('zip://' . __DIR__ . '/test_with_comment.zip#foo', 'rb');
if (!$fp) {
  exit("cannot open\n");
}
$contents = '';
while (!feof($fp)) {
  $contents .= fread($fp, 2);
}
var_dump($contents);
fclose($fp);

?>
--EXPECTF--
resource(%d) of type (stream)
string(5) "foo

"
string(5) "foo

"
