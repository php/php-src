--TEST--
Bug #81122 (SSRF bypass in FILTER_VALIDATE_URL)
--EXTENSIONS--
filter
--FILE--
<?php
$urls = [
    "https://example.com:\\@test.com/",
    "https://user:\\epass@test.com",
    "https://user:\\@test.com",
];
foreach ($urls as $url) {
    var_dump(filter_var($url, FILTER_VALIDATE_URL));
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
