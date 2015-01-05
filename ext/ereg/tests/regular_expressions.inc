<?php

/**
 * POSIX regular expressions each coupled with a string that they match,
 * based on documentation on http://www.tin.org/bin/man.cgi?section=7&topic=regex .
 */ 
$expressions = array(
  //array(pattern, string to match)
    array('..(a|b|c)(a|b|c)..', '--- ab ---'),
    array('()', ''),
    array('()', 'abcdef'),
    array('[x]|[^x]', 'abcdef'),
    array('(a{1})(a{1,}) (b{1,3}) (c+) (d?ddd|e)', '--- aaa bbb ccc ddd ---'),
    array('\\\\\`\^\.\[\$\(\)\|\*\+\?\{\\\'', '\\`^.[$()|*+?{\''),
    array('\\a', 'a'),
    array('[0-9][^0-9]', '2a'),
    array('^[[:alnum:]]{62,62}$', '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'),
    array('^[[:digit:]]{5}', '0123456789'),
	array('[[:digit:]]{5}$', '0123456789'),
	array('[[:blank:]]{1,10}', "\n \t"),
	array('[[:print:]]{3}', " a "),
);

?>