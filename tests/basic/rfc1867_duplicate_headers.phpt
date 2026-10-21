--TEST--
RFC1867 retains the first relevant header and ignores unused headers
--INI--
file_uploads=1
--POST_RAW--
Content-Type: multipart/form-data; boundary=php71-security
--php71-security
Content-Disposition: form-data; name="field"
X-Unused: ignored
Content-Disposition: form-data; name="wrong"

value
--php71-security
Content-Disposition: form-data; name="upload"; filename="first.txt"
Content-Disposition: form-data; name="wrong"; filename="second.txt"
Content-Type: text/plain
Content-Type: application/octet-stream
X-Unused: ignored

hello
--php71-security--
--FILE--
<?php
var_dump($_POST, $_FILES['upload']['name'], $_FILES['upload']['type'],
	$_FILES['upload']['size'], $_FILES['upload']['error']);
?>
--EXPECT--
array(1) {
  ["field"]=>
  string(5) "value"
}
string(9) "first.txt"
string(10) "text/plain"
int(5)
int(0)
