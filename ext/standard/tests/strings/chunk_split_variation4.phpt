--TEST--
Test chunk_split() function : usage variations - different heredoc strings as 'str' argument
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

/*
* Passing different heredoc strings as 'str' argument to the chunk_split()
* with 'chunklen' 4 and default value of 'ending' that is "\r\n"
*/

echo "*** Testing chunk_split() : heredoc strings as 'str' argument ***\n";

// Initializing required variables
$chunklen = 4;

// Null heredoc string
$heredoc_null = <<<EOT1
EOT1;

// heredoc string with single character
$heredoc_char = <<<EOT2
a
EOT2;

// simple heredoc string
$heredoc_str = <<<EOT3
This is simple heredoc string
EOT3;

// heredoc with special characters
$heredoc_spchar = <<<EOT4
This checks heredoc with $, %, &, chars
EOT4;

// blank heredoc string
$heredoc_blank = <<<EOT5

EOT5;

// heredoc with different white space characters
$heredoc_escchar = <<<EOT6
This checks\t chunk_split()\nEscape\rchars
EOT6;

// heredoc with multiline
$heredoc_multiline= <<<EOT7
This is to check chunk_split
function with multiline
heredoc
EOT7;

// heredoc with quotes and slashes
$heredoc_quote_slash = <<<EOT8
"To check " in heredoc"
I'm sure it'll work also with \
which is single slash
EOT8;

//different heredoc strings for 'str'
$heredoc_arr = array(
  $heredoc_null,
  $heredoc_blank,
  $heredoc_char,
  $heredoc_str,
  $heredoc_multiline,
  $heredoc_spchar,
  $heredoc_escchar,
  $heredoc_quote_slash
);


// loop through each element of the heredoc_arr for 'str'
$count = 0;
foreach($heredoc_arr as $str) {
  echo "-- Iteration ".($count+1). " --\n";
  var_dump( chunk_split( $str, $chunklen) );
  $count++;
};

echo "Done"
?>
--EXPECT--
*** Testing chunk_split() : heredoc strings as 'str' argument ***
-- Iteration 1 --
string(2) "
"
-- Iteration 2 --
string(2) "
"
-- Iteration 3 --
string(3) "a
"
-- Iteration 4 --
string(45) "This
 is 
simp
le h
ered
oc s
trin
g
"
-- Iteration 5 --
string(90) "This
 is 
to c
heck
 chu
nk_s
plit

fun
ctio
n wi
th m
ulti
line

her
edoc
"
-- Iteration 6 --
string(59) "This
 che
cks 
here
doc 
with
 $, 
%, &
, ch
ars
"
-- Iteration 7 --
string(59) "This
 che
cks	
 chu
nk_s
plit
()
E
scap
ech
ars
"
-- Iteration 8 --
string(117) ""To 
chec
k " 
in h
ered
oc"

I'm 
sure
 it'
ll w
ork 
also
 wit
h \

whic
h is
 sin
gle 
slas
h
"
Done
