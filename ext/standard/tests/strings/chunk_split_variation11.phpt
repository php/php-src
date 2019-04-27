--TEST--
Test chunk_split() function : usage variations - different strings for 'ending' with heredoc 'str'
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* passing different strings for 'ending' and heredoc string as 'str' to chunk_split()
* 'chunklen' is set to 6E0 for this testcase
*/

echo "*** Testing chunk_split() : different values for 'ending' with heredoc 'str'***\n";

// Initializing required variables
// heredoc string for 'str' argument
$heredoc_str = <<<EOT
This is heredoc string with \t and \n.It also contains
sPeci@! ch@r$ :) & numbers 222.This is \k wrong escape char.
EOT;

$chunklen = 6E+0;

//different values for 'ending'
$values = array (
  "",  //empty
  " ",  //space
  "a",  //single char
  "ENDING",  //regular string
  "\r\n",  //White space char
  "123",  //Numeric
  ")speci@! ch@r$(",  //String with special chars
);

//loop through each values for 'ending'
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count+1). " --\n";
  var_dump( chunk_split($heredoc_str, $chunklen, $values[$count]) );
}

echo "Done"
?>
--EXPECT--
*** Testing chunk_split() : different values for 'ending' with heredoc 'str'***
-- Iteration 1 --
string(113) "This is heredoc string with 	 and 
.It also contains
sPeci@! ch@r$ :) & numbers 222.This is \k wrong escape char."
-- Iteration 2 --
string(132) "This i s here doc st ring w ith 	  and 
. It als o cont ains
s Peci@!  ch@r$  :) &  number s 222. This i s \k w rong e scape  char. "
-- Iteration 3 --
string(132) "This ias hereadoc staring waith 	 aand 
.aIt alsao contaains
saPeci@!a ch@r$a :) & anumberas 222.aThis ias \k warong eascape achar.a"
-- Iteration 4 --
string(227) "This iENDINGs hereENDINGdoc stENDINGring wENDINGith 	 ENDINGand 
.ENDINGIt alsENDINGo contENDINGains
sENDINGPeci@!ENDING ch@r$ENDING :) & ENDINGnumberENDINGs 222.ENDINGThis iENDINGs \k wENDINGrong eENDINGscape ENDINGchar.ENDING"
-- Iteration 5 --
string(151) "This i
s here
doc st
ring w
ith 	 
and 
.
It als
o cont
ains
s
Peci@!
 ch@r$
 :) & 
number
s 222.
This i
s \k w
rong e
scape 
char.
"
-- Iteration 6 --
string(170) "This i123s here123doc st123ring w123ith 	 123and 
.123It als123o cont123ains
s123Peci@!123 ch@r$123 :) & 123number123s 222.123This i123s \k w123rong e123scape 123char.123"
-- Iteration 7 --
string(398) "This i)speci@! ch@r$(s here)speci@! ch@r$(doc st)speci@! ch@r$(ring w)speci@! ch@r$(ith 	 )speci@! ch@r$(and 
.)speci@! ch@r$(It als)speci@! ch@r$(o cont)speci@! ch@r$(ains
s)speci@! ch@r$(Peci@!)speci@! ch@r$( ch@r$)speci@! ch@r$( :) & )speci@! ch@r$(number)speci@! ch@r$(s 222.)speci@! ch@r$(This i)speci@! ch@r$(s \k w)speci@! ch@r$(rong e)speci@! ch@r$(scape )speci@! ch@r$(char.)speci@! ch@r$("
Done
