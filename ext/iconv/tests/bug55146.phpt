--TEST--
Bug #55146 (iconv_mime_decode_headers() skips some headers)
--EXTENSIONS--
iconv
--FILE--
<?php

$headers = <<< HEADERS
X-Header-One: H4sIAAAAAAAAA+NgFlsCAAA=
X-Header-Two: XtLePq6GTMn8G68F0
HEADERS;
var_dump(iconv_mime_decode_headers($headers, ICONV_MIME_DECODE_CONTINUE_ON_ERROR));

$headers = <<< HEADERS
X-Header-One: =
X-Header-Two: XtLePq6GTMn8G68F0
HEADERS;
var_dump(iconv_mime_decode_headers($headers, ICONV_MIME_DECODE_STRICT));
?>
--EXPECT--
array(2) {
  ["X-Header-One"]=>
  string(24) "H4sIAAAAAAAAA+NgFlsCAAA="
  ["X-Header-Two"]=>
  string(17) "XtLePq6GTMn8G68F0"
}
array(2) {
  ["X-Header-One"]=>
  string(1) "="
  ["X-Header-Two"]=>
  string(17) "XtLePq6GTMn8G68F0"
}
