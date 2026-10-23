--TEST--
GH-24492: URL encoding output is unchanged with fallible allocations
--FILE--
<?php
foreach ([0, 1, 15, 16, 17, 256] as $length) {
    $prefix = str_repeat('a', $length);
    $input = $prefix . " \0~+/%";
    var_dump(urlencode($input) === $prefix . '+%00%7E%2B%2F%25');
    var_dump(rawurlencode($input) === $prefix . '%20%00~%2B%2F%25');
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
