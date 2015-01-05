--TEST--
Test str_replace() function for case (needle=string ; replace=array)
--INI--
precision=14
--FILE--
<?php
/* 
  Prototype: mixed str_replace(mixed $search, mixed $replace, 
                               mixed $subject [, int &$count]);
  Description: Replace all occurrences of the search string with 
               the replacement string

  This feature was requested in bug #38685: if needle is a string and replace
      is an array, each  occurence of needle is replaced by an element of the
	  replace array. If the replace array contains less elements than the
	  count of needles found, loop to the first one and continue.
	  Before this feature was implemented, when the needle was a string, the
	  replace arg was forced to a string.
*/

function check_replace($search,$replace,&$subject)
{
$c=0;
var_dump($subject=str_replace($search,$replace,$subject,$c));
echo "Count: $c\n";
}

//----

function msg($str)
{
echo "\n--- $str ---\n\n";
}

//----

msg("1 element");

$search='[?]';
$repl=array('alpha','beta',10, null);
$subject='Replace 1 [?] element';
check_replace($search,$repl,$subject);

msg("replace count, grouped");

$subject='Replace [?][?][?][?] repl/count grouped elements';
check_replace($search,$repl,$subject);

msg("replace count, separate");

$subject='Replace [?] repl/count [?] separate [?] elem[?]ents';
check_replace($search,$repl,$subject);

msg("replace loop, grouped");

$subject='Replace [?][?][?][?][?][?][?][?][?][?][?][?][?][?][?][?][?] a lot of grouped elements';
check_replace($search,$repl,$subject);

msg("eplace loop, separate");

$subject='[?]Rep[?]la[?]ce [?] ma[?]ny[?] [?] s[?]epa[?]rate [?] el[?]emen[?]ts[?]?';
check_replace($search,$repl,$subject);

msg("Empty replace array");

$subject='[?]Rep[?]la[?]ce[?] wi[?]th[?][?][?][?] e[?]mp[?]ty [?]ar[?]ray[?][?]';
$repl=array();
check_replace($search,$repl,$subject);

msg("replace elements contain needle");

$subject='[?]Rep[?]la[?]ce [?]ma[?]ny[?] [?]s[?]epa[?]rate[?] el[?]emen[?]ts[?]?';
$repl=array('al[?]pz[?]',"[?]b[?]v\n",null,37,"[?]\n[?]");
check_replace($search,$repl,$subject);
check_replace($search,$repl,$subject);
check_replace($search,$repl,$subject);

msg("subject == needle");

$search='FOOOOOOOOOOOOOOOOOOOOOOOOOO';
$subject=$search;
$repl=array(1,2,3);
check_replace($search,$repl,$subject);

msg("length(subject) < length(needle)");

$subject='FOOOOOOOOOOOOOOOOOOOOOOOOOO';
$search=$subject."\n";
$repl=array(1,2,3);
check_replace($search,$repl,$subject);

?>
===DONE===
--EXPECTF--	
--- 1 element ---

string(23) "Replace 1 alpha element"
Count: 1

--- replace count, grouped ---

string(47) "Replace alphabeta10 repl/count grouped elements"
Count: 4

--- replace count, separate ---

string(50) "Replace alpha repl/count beta separate 10 elements"
Count: 4

--- replace loop, grouped ---

string(83) "Replace alphabeta10alphabeta10alphabeta10alphabeta10alpha a lot of grouped elements"
Count: 17

--- eplace loop, separate ---

string(72) "alphaRepbetala10ce  maalphanybeta 10 sepaalpharate beta el10ementsalpha?"
Count: 13

--- Empty replace array ---

string(24) "Replace with empty array"
Count: 15

--- replace elements contain needle ---

string(106) "al[?]pz[?]Rep[?]b[?]v
lace 37ma[?]
[?]nyal[?]pz[?] [?]b[?]v
sepa37rate[?]
[?] elal[?]pz[?]emen[?]b[?]v
ts?"
Count: 13
string(152) "alal[?]pz[?]pz[?]b[?]v
Repb37v
lace 37ma[?]
[?]
al[?]pz[?]nyal[?]b[?]v
pz 37b[?]
[?]v
sepa37rateal[?]pz[?]
[?]b[?]v
 elalpz37emen[?]
[?]bal[?]pz[?]v
ts?"
Count: 16
string(204) "alalal[?]pz[?]pz[?]b[?]v
pzb37v
Repb37v
lace 37ma[?]
[?]
al[?]pz[?]
al[?]b[?]v
pznyal37b[?]
[?]v
pz 37bal[?]pz[?]
[?]b[?]v
v
sepa37ratealpz37
[?]
[?]bal[?]pz[?]v
 elalpz37emen[?]b[?]v

bal37pz[?]
[?]v
ts?"
Count: 20

--- subject == needle ---

string(1) "1"
Count: 1

--- length(subject) < length(needle) ---

string(27) "FOOOOOOOOOOOOOOOOOOOOOOOOOO"
Count: 0
===DONE===