--TEST--
Bug #77423 (parse_url() will deliver a wrong host to user)
--SKIPIF--
<?php if (!function_exists('filter_var')) { echo "skip requires filter\n"; } ?>
--FILE--
<?php
$urls = array(
    "http://php.net\@aliyun.com/aaa.do",
    "https://example.com\uFF03@bing.com",
);
foreach ($urls as $url) {
    var_dump(filter_var($url, FILTER_VALIDATE_URL));
}
?>
--EXPECT--
bool(false)
bool(false)
