--TEST--
Test match jumptable with only one arm
--FILE--
<?php

try {
    var_dump(match(true) {
        1, 2, 3, 4, 5 => 'foo',
    });
} catch (Error $e) {
    var_dump((string) $e);
}

try {
    var_dump(match(6) {
        1, 2, 3, 4, 5 => 'foo',
    });
} catch (Error $e) {
    var_dump((string) $e);
}

try {
    var_dump(match('3') {
        1, 2, 3, 4, 5 => 'foo',
    });
} catch (Error $e) {
    var_dump((string) $e);
}

var_dump(match(3) {
    1, 2, 3, 4, 5 => 'foo',
});

var_dump(match(true) {
    1, 2, 3, 4, 5 => 'foo',
    default => 'bar',
});

var_dump(match(6) {
    1, 2, 3, 4, 5 => 'foo',
    default => 'bar',
});

var_dump(match('3') {
    1, 2, 3, 4, 5 => 'foo',
    default => 'bar',
});

var_dump(match(3) {
    1, 2, 3, 4, 5 => 'foo',
    default => 'bar',
});

--EXPECTF--
string(%d) "UnhandledMatchError in %s037.php:5
Stack trace:
#0 {main}"
string(%d) "UnhandledMatchError in %s037.php:13
Stack trace:
#0 {main}"
string(%d) "UnhandledMatchError in %s037.php:21
Stack trace:
#0 {main}"
string(3) "foo"
string(3) "bar"
string(3) "bar"
string(3) "bar"
string(3) "foo"
