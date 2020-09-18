--TEST--
pspell basic tests (warning: may fail with pspell/aspell < GNU Aspell 0.50.3)
--SKIPIF--
<?php
if (!extension_loaded("pspell")) die("skip");
if (!@pspell_new ("en", "", "", "", (PSPELL_FAST|PSPELL_RUN_TOGETHER))) {
    die("skip English dictionary is not available");
}
?>
--FILE--
<?php

error_reporting(E_ALL);
$string = "";
$string .= "I will not buy this record, it is scratched. ";
$string .= "Sorry ";
$string .= "I will not buy this record, it is scratched. ";
$string .= "Uh, no, no, no. This is a tobacconist's ";
$string .= "Ah! I will not buy this tobacconist's, it is scratched. ";
$string .= "No, no, no, no. Tobacco... um... cigarettes (holds up a pack). ";
$string .= "Ya! See-gar-ets! Ya! Uh... My hovercraft is full of eels. ";
$string .= "Sorry? ";
$string .= "My hovercraft (pantomimes puffing a cigarette)... is full of eels (pretends to strike a match). ";
$string .= "Ahh, matches!";

$pspell = pspell_new ("en", "", "", "", (PSPELL_FAST|PSPELL_RUN_TOGETHER));
$array = explode(' ',preg_replace('/[^a-zA-Z0-9 ]/','',$string));
for($i=0,$u=count($array);$i<$u;++$i) {
    echo $array[$i].' : ';
    if (!pspell_check($pspell, $array[$i])) {
        echo "..false\n";
        echo "Possible spellings: " . join(',',pspell_suggest ($pspell, $array[$i])) . "\n";
    } else {
        echo "true\n";
    }
}
?>
--EXPECTF--
I : true
will : true
not : true
buy : true
this : true
record : true
it : true
is : true
scratched : true
Sorry : true
I : true
will : true
not : true
buy : true
this : true
record : true
it : true
is : true
scratched : true
Uh : true
no : true
no : true
no : true
This : true
is : true
a : true
tobacconists : true
Ah : true
I : true
will : true
not : true
buy : true
this : true
tobacconists : true
it : true
is : true
scratched : true
No : true
no : true
no : true
no : true
Tobacco : true
um : true
cigarettes : true
holds : true
up : true
a : true
pack : true
Ya : true
Seegarets : ..false
Possible spellings:%s,Regrets,%s,Cigarettes,%s
Ya : true
Uh : true
My : true
hovercraft : true
is : true
full : true
of : true
eels : true
Sorry : true
My : true
hovercraft : true
pantomimes : true
puffing : true
a : true
cigarette : true
is : true
full : true
of : true
eels : true
pretends : true
to : true
strike : true
a : true
match : true
Ahh : ..false
Possible spellings:%sAh,Aha,%s
matches : true
