--TEST--
$http_reponse_header should warn once per file
--SKIPIF--
<?php require 'server.inc'; http_server_skipif(); ?>
--INI--
allow_url_fopen=1
--FILE--
<?php
require 'server.inc';

include 'http_response_header_deprecated_multiple_files_1.inc';

include 'http_response_header_deprecated_multiple_files_2.inc';

?>
--EXPECTF--
Deprecated: The predefined locally scoped $http_response_header variable is deprecated, call http_get_last_response_headers() instead in %shttp_response_header_deprecated_multiple_files_1.inc on line %d
string(6) "Body11"
array(2) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(13) "Some: Header1"
}

Deprecated: The predefined locally scoped $http_response_header variable is deprecated, call http_get_last_response_headers() instead in %shttp_response_header_deprecated_multiple_files_2.inc on line %d
string(5) "Body2"
array(2) {
  [0]=>
  string(15) "HTTP/1.0 200 Ok"
  [1]=>
  string(13) "Some: Header2"
}
