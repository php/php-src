--TEST--
glob() negates a bracket expression that starts with "!" or "^"
--FILE--
<?php
$dir = __DIR__ . '/glob_bracket_negation';
@mkdir($dir);
foreach (['a', 'b', 'c', '^', '!'] as $name) {
    touch("$dir/$name");
}

foreach (['[!a]', '[^a]', '[!a-b]', '[^a-b]', '[!]a]', '[^]a]', '[a^]'] as $pattern) {
    echo $pattern, ': ', implode(' ', array_map('basename', glob("$dir/$pattern"))), "\n";
}
?>
--CLEAN--
<?php
$dir = __DIR__ . '/glob_bracket_negation';
foreach (['a', 'b', 'c', '^', '!'] as $name) {
    @unlink("$dir/$name");
}
@rmdir($dir);
?>
--EXPECT--
[!a]: ! ^ b c
[^a]: ! ^ b c
[!a-b]: ! ^ c
[^a-b]: ! ^ c
[!]a]: ! ^ b c
[^]a]: ! ^ b c
[a^]: ^ a
