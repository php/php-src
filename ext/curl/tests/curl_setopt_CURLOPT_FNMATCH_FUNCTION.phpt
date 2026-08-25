--TEST--
Curl option CURLOPT_FNMATCH_FUNCTION
--EXTENSIONS--
curl
--FILE--
<?php

require(__DIR__ . '/../../ftp/tests/server.inc');

$seen_patterns = [];
$seen_fnames = [];

function fnmatch_function($ch, string $pattern, string $fname) {
    global $seen_patterns, $seen_fnames;

    $seen_patterns[] = $pattern;
    $seen_fnames[] = $fname;

    if ($fname === 'a story') {
        return CURL_FNMATCHFUNC_MATCH;
    } else {
        return CURL_FNMATCHFUNC_NOMATCH;
    }
}

$ch = curl_init("ftp://$socket_name/f*");
curl_setopt($ch, CURLOPT_FNMATCH_FUNCTION, 'fnmatch_function');
curl_setopt($ch, CURLOPT_WILDCARDMATCH, 1);
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
echo curl_exec($ch);

var_dump(in_array('f*', $seen_patterns));
var_dump(in_array('mediumfile', $seen_fnames));

?>
--EXPECT--
For sale: baby shoes, never worn.
bool(true)
bool(true)
