--TEST--
PCRE re-entrancy: nested calls should not corrupt global match data
--EXTENSIONS--
pcre
--FILE--
<?php

echo "Testing nested PCRE calls..." . PHP_EOL;

$subject = 'abc';

// preg_replace_callback is the most common way to trigger re-entrancy
$result = preg_replace_callback('/./', function($m) {
    $char = $m[0];
    echo "Outer match: $char" . PHP_EOL;

    // 1. Nested preg_match
    preg_match('/./', 'inner', $inner_m);

    // 2. Nested preg_replace (string version)
    preg_replace('/n/', 'N', 'inner');

    // 3. Nested preg_split
    preg_split('/n/', 'inner');

    // 4. Nested preg_grep
    preg_grep('/n/', ['inner']);

    // If any of the above stole the global mdata buffer without setting mdata_used,
    // the 'offsets' used by this outer preg_replace_callback loop would be corrupted.

    return strtoupper($char);
}, $subject);

var_dump($result);

echo PHP_EOL . "Testing deep nesting..." . PHP_EOL;

$result = preg_replace_callback('/a/', function($m) {
    return preg_replace_callback('/b/', function($m) {
        return preg_replace_callback('/c/', function($m) {
            return "SUCCESS";
        }, 'c');
    }, 'b');
}, 'a');

var_dump($result);

?>
--EXPECT--
Testing nested PCRE calls...
Outer match: a
Outer match: b
Outer match: c
string(3) "ABC"

Testing deep nesting...
string(7) "SUCCESS"
