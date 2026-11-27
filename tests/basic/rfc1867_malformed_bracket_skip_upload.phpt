--TEST--
rfc1867: malformed bracket in one file part name does not skip subsequent valid file uploads
--INI--
file_uploads=1
--POST_RAW--
Content-Type: multipart/form-data; boundary=----PhpTestBoundaryH8
------PhpTestBoundaryH8
Content-Disposition: form-data; name="bad["; filename="bad.txt"
Content-Type: text/plain

bad-body
------PhpTestBoundaryH8
Content-Disposition: form-data; name="good"; filename="good.txt"
Content-Type: text/plain

good-body
------PhpTestBoundaryH8
Content-Disposition: form-data; name="field_after"

ok
------PhpTestBoundaryH8--
--FILE--
<?php
var_dump(array_keys($_POST));
var_dump(array_keys($_FILES));
var_dump($_FILES['good']['name'] ?? null);
var_dump($_FILES['good']['size'] ?? null);
?>
--EXPECT--
array(1) {
  [0]=>
  string(11) "field_after"
}
array(1) {
  [0]=>
  string(4) "good"
}
string(8) "good.txt"
int(9)
