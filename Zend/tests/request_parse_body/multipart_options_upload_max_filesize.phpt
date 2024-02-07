--TEST--
request_parse_body() upload_max_filesize option
--INI--
upload_max_filesize=1M
--ENV--
REQUEST_METHOD=PUT
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------84000087610663814162942123332
-----------------------------84000087610663814162942123332
Content-Disposition: name="file1"; filename="file1.txt"

aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
-----------------------------84000087610663814162942123332--
--FILE--
<?php

try {
    [$_POST, $_FILES] = request_parse_body([
        'upload_max_filesize' => '128',
    ]);
} catch (Exception $e) {
    echo get_class($e), ': ', $e->getMessage(), "\n";
}

var_dump($_POST, $_FILES);

?>
--EXPECT--
array(0) {
}
array(1) {
  ["file1"]=>
  array(6) {
    ["name"]=>
    string(9) "file1.txt"
    ["full_path"]=>
    string(9) "file1.txt"
    ["type"]=>
    string(0) ""
    ["tmp_name"]=>
    string(0) ""
    ["error"]=>
    int(1)
    ["size"]=>
    int(0)
  }
}
