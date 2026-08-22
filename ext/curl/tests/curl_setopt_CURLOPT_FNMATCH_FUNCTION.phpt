--TEST--
Curl option CURLOPT_FNMATCH_FUNCTION
--EXTENSIONS--
curl
--SKIPIF--
<?php
if (getenv('SKIP_ONLINE_TESTS')) die('skip Online test');
?>
--FILE--
<?php

$seen_patterns = [];
$seen_fnames = [];

function fnmatch_function($ch, string $pattern, string $fname) {
    global $seen_patterns, $seen_fnames;

    $seen_patterns[] = $pattern;
    $seen_fnames[] = $fname;

    if ($fname === 'README.mirrors.txt') {
        return CURL_FNMATCHFUNC_MATCH;
    } else {
        return CURL_FNMATCHFUNC_NOMATCH;
    }
}

$ch = curl_init('ftp://debian.snt.utwente.nl/debian/README.*');
curl_setopt($ch, CURLOPT_FNMATCH_FUNCTION, 'fnmatch_function');
curl_setopt($ch, CURLOPT_WILDCARDMATCH, 1);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
echo curl_exec($ch);

var_dump(in_array('README.*', $seen_patterns));
var_dump(in_array('README.html', $seen_fnames));

?>
--EXPECT--
The list of Debian mirror sites is available here: https://www.debian.org/mirror/list
bool(true)
bool(true)
