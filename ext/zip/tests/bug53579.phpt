--TEST--
Bug #53579 (stream_get_contents() segfaults on ziparchive streams)
--SKIPIF--
<?php
/* $Id: oo_stream.phpt 260091 2008-05-21 09:27:41Z pajoye $ */
if(!extension_loaded('zip')) die('skip');
?>
--FILE--
<?php
$dirname = dirname(__FILE__) . '/';
$file = $dirname . 'test_with_comment.zip';
include $dirname . 'utils.inc';
$zip = new ZipArchive;
if (!$zip->open($file)) {
	exit('failed');
}
$fp = $zip->getStream('foo');

var_dump($fp);
if(!$fp) exit("\n");
$contents = stream_get_contents($fp);

fclose($fp);
$zip->close();
var_dump($contents);


$fp = fopen('zip://' . dirname(__FILE__) . '/test_with_comment.zip#foo', 'rb');
if (!$fp) {
  exit("cannot open\n");
}
$contents = stream_get_contents($fp);
var_dump($contents);
fclose($fp);

?>
--EXPECTF--
resource(%d) of type (stream)
string(5) "foo

"
string(5) "foo

"
