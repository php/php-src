--TEST--
Check handling of multiple To headers
--SKIPIF--
<?php if (!extension_loaded("mailparse")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
$text = <<<EOD
To: fred@bloggs.com
To: wez@thebrainroom.com

hello, this is some text=hello.
EOD;

$mime = mailparse_msg_create();
mailparse_msg_parse($mime, $text);
$data = mailparse_msg_get_part_data($mime);
echo $data["headers"]["to"];
?>
--EXPECT--
fred@bloggs.com, wez@thebrainroom.com
