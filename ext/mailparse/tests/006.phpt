--TEST--
Check extract_part_file
--SKIPIF--
<?php if (!extension_loaded("mailparse")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php 
$text = <<<EOD
To: fred@bloggs.com
Mime-Version: 1.0
Content-Type: text/plain
Subject: A simple MIME message

hello, this is some text hello.
blah blah blah.

EOD;

$fp = tmpfile();
fwrite($fp, $text);
rewind($fp);


$mime = mailparse_msg_create();
mailparse_msg_parse($mime, $text);

echo "Extract to output\n";
mailparse_msg_extract_part_file($mime, $fp);

echo "Extract and return as string\n";
$result = mailparse_msg_extract_part_file($mime, $fp, null);
echo "-->\n";
echo $result;

echo "Extract to open file\n";
$fpdest = tmpfile();
mailparse_msg_extract_part_file($mime, $fp, $fpdest);
echo "rewinding\n";
rewind($fpdest);
fpassthru($fpdest);

echo "Extract via user function\n";
$cbdata = "";
function callback($data) {
	$GLOBALS["cbdata"] .= $data;
}
mailparse_msg_extract_part_file($mime, $fp, "callback");
echo "callback data is:\n";
echo $cbdata;

echo "Extract whole part to output\n";
mailparse_msg_extract_whole_part_file($mime, $fp);

echo "Extract part from string to output\n";
mailparse_msg_extract_part($mime, $text);
fclose($fpdest);
fclose($fp);

?>
--EXPECT--
Extract to output
hello, this is some text hello.
blah blah blah.
Extract and return as string
-->
hello, this is some text hello.
blah blah blah.
Extract to open file
rewinding
hello, this is some text hello.
blah blah blah.
Extract via user function
callback data is:
hello, this is some text hello.
blah blah blah.
Extract whole part to output
To: fred@bloggs.com
Mime-Version: 1.0
Content-Type: text/plain
Subject: A simple MIME message

hello, this is some text hello.
blah blah blah.
Extract part from string to output
hello, this is some text hello.
blah blah blah.

