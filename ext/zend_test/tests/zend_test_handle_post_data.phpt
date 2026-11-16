--TEST--
zend_test_handle_post_data() parses POST data from a stream
--EXTENSIONS--
zend_test
--FILE--
<?php

function stream(string $body) {
    $stream = fopen('php://memory', 'r+');
    fwrite($stream, $body);
    rewind($stream);
    return $stream;
}

$body = "--b\r\n"
    . "Content-Disposition: form-data; name=\"foo\"\r\n"
    . "\r\n"
    . "bar\r\n"
    . "--b\r\n"
    . "Content-Disposition: form-data; name=\"file\"; filename=\"f.txt\"\r\n"
    . "Content-Type: text/plain\r\n"
    . "\r\n"
    . "file contents\r\n"
    . "--b--\r\n";

zend_test_handle_post_data(stream($body), 'multipart/form-data; boundary=b', strlen($body));

var_dump($_POST);
var_dump($_FILES);
var_dump(is_uploaded_file($_FILES['file']['tmp_name']));
var_dump(file_get_contents($_FILES['file']['tmp_name']));

// Each call starts over with a fresh $_POST and $_FILES.
$body = "a=1&b[]=2&b[]=3";
zend_test_handle_post_data(stream($body), 'application/x-www-form-urlencoded', strlen($body));

var_dump($_POST);
var_dump($_FILES);

// No handler for this content type, the body is swallowed.
$body = '{"a":1}';
zend_test_handle_post_data(stream($body), 'application/json', strlen($body));

var_dump($_POST);

?>
--EXPECTF--
array(1) {
  ["foo"]=>
  string(3) "bar"
}
array(1) {
  ["file"]=>
  array(6) {
    ["name"]=>
    string(5) "f.txt"
    ["full_path"]=>
    string(5) "f.txt"
    ["type"]=>
    string(10) "text/plain"
    ["tmp_name"]=>
    string(%d) "%s"
    ["error"]=>
    int(0)
    ["size"]=>
    int(13)
  }
}
bool(true)
string(13) "file contents"
array(2) {
  ["a"]=>
  string(1) "1"
  ["b"]=>
  array(2) {
    [0]=>
    string(1) "2"
    [1]=>
    string(1) "3"
  }
}
array(0) {
}
array(0) {
}
