--TEST--
Bug#55504 (Content-Type header is not parsed correctly on HTTP POST request)
--INI--
file_uploads=1
--POST_RAW--
Content-Type: multipart/form-data; boundary=BVoyv; charset=iso-8859-1
--BVoyv
Content-Disposition: form-data; name="data"

abc
--BVoyv
Content-Disposition: form-data; name="data2"

more data
--BVoyv
Content-Disposition: form-data; name="data3"

even more data
--BVoyv--
--FILE--
<?php
var_dump($_POST);
?>
--EXPECT--
array(3) {
  ["data"]=>
  string(3) "abc"
  ["data2"]=>
  string(9) "more data"
  ["data3"]=>
  string(14) "even more data"
}
