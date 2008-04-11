<?php

/**
 * POSIX regular expressions each coupled with a string that they match,
 * based on documentation on http://www.tin.org/bin/man.cgi?section=7&topic=regex .
 */ 
$expressions = array(
  //array(pattern, string to match)
    array(b'..(a|b|c)(a|b|c)..', '--- ab ---'),
    array(b'()', ''),
    array(b'()', 'abcdef'),
    array(b'[x]|[^x]', 'abcdef'),
    array(b'(a{1})(a{1,}) (b{1,3}) (c+) (d?ddd|e)', '--- aaa bbb ccc ddd ---'),
    array(b'\\\\\`\^\.\[\$\(\)\|\*\+\?\{\\\'', '\\`^.[$()|*+?{\''),
    array(b'\\a', 'a'),
    array(b'[0-9][^0-9]', '2a'),
    array(b'^[[:alnum:]]{62,62}$', '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'),
    array(b'^[[:digit:]]{5}', '0123456789'),
	array(b'[[:digit:]]{5}$', '0123456789'),
	array(b'[[:blank:]]{1,10}', "\n \t"),
	array(b'[[:print:]]{3}', " a "),
);

?>