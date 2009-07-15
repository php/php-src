--TEST--
Test nl2br() function : usage variations - heredoc strings for 'str' argument
--FILE--
<?php
/* Prototype  : string nl2br(string $str);
 * Description: Inserts HTML line breaks before all newlines in a string.
 * Source code: ext/standard/string.c
*/

/* Test nl2br() function by passing heredoc strings containing various
 *   combinations of new line chars to 'str' argument
*/

echo "*** Testing nl2br() : usage variations ***\n";
//heredoc string containing new line chars(\n, \r and combinations of \r & \n) and new lines
$heredoc_str1 = <<<EOD
\n
\r
\r\n
\nnn\n\n\nn
\rrr\r\r\rr
\n\r\n\r\r\n\nr\rn
EOD;

//heredoc string containing embedded 'new line chars'/'new lines' in the string
$heredoc_str2 = <<<EOD
Hello\nWorld\r
This is \tes\t for \n \new lines
like \n \r\n \r \n\r and etc
EOD;

var_dump(nl2br($heredoc_str1) );
var_dump(nl2br($heredoc_str2) );

echo "Done";
?>
--EXPECTF--
*** Testing nl2br() : usage variations ***
string(147) "<br />
<br />
<br />
<br />
<br />
<br />
nn<br />
<br />
<br />
n<br />
rr<br /><br /><br />r<br />
<br />
<br />
<br />
<br />
r<br />n"
string(118) "Hello<br />
World<br />
This is 	es	 for <br />
 <br />
ew lines<br />
like <br />
 <br />
 <br /> <br />
 and etc"
Done
