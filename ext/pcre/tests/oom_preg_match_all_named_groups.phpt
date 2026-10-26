--TEST--
preg_match_all() with named groups keeps the cached subpattern names on OOM
--INI--
memory_limit=16M
--FILE--
<?php
register_shutdown_function(function () {
    preg_match_all('/(?<l>[a-z])(?<d>\d)/', 'a1b2', $matches);
    echo json_encode($matches), "\n";
});

$subject = str_repeat("ab12 ", 400000);
preg_match_all('/(?<l>[a-z])(?<d>\d)/', $subject, $matches);
?>
--EXPECTF--
Fatal error: Allowed memory size of 16777216 bytes exhausted %S(tried to allocate %d bytes) in %s on line %d
{"0":["a1","b2"],"l":["a","b"],"1":["a","b"],"d":["1","2"],"2":["1","2"]}
