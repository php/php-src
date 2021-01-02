--TEST--
Bug #77423 (parse_url() will deliver a wrong host to user)
--FILE--
<?php
$urls = array(
    "http://php.net\@aliyun.com/aaa.do",
    "https://example.com\uFF03@bing.com",
);
foreach ($urls as $url) {
    var_dump(filter_var($url, FILTER_VALIDATE_URL));
    var_dump(parse_url($url));
}
?>
--EXPECT--
bool(false)
array(3) {
  ["scheme"]=>
  string(4) "http"
  ["host"]=>
  string(19) "php.net\@aliyun.com"
  ["path"]=>
  string(7) "/aaa.do"
}
bool(false)
array(2) {
  ["scheme"]=>
  string(5) "https"
  ["host"]=>
  string(26) "example.com\uFF03@bing.com"
}
