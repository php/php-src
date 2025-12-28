--TEST--
GH-16523 (FILTER_FLAG_HOSTNAME accepts ending hyphen)
--EXTENSIONS--
filter
--FILE--
<?php
$domains = [
    'a-.bc.com',
    'a.bc-.com',
    'a.bc.com-',
];

foreach ($domains as $domain) {
    var_dump(filter_var($domain, FILTER_VALIDATE_DOMAIN, FILTER_FLAG_HOSTNAME));
}
?>
--EXPECT--
bool(false)
bool(false)
bool(false)
