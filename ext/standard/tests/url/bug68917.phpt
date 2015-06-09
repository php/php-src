--TEST--
Bug #68917 (parse_url fails on some partial urls)
--FILE--
<?php
print_r(parse_url('//example.org:81/hi?a=b#c=d'));
print_r(parse_url('//example.org/hi?a=b#c=d'));
?>
--EXPECT--
Array
(
    [host] => example.org
    [port] => 81
    [path] => /hi
    [query] => a=b
    [fragment] => c=d
)
Array
(
    [host] => example.org
    [path] => /hi
    [query] => a=b
    [fragment] => c=d
)
