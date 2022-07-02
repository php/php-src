--TEST--
soundex() tests
--FILE--
<?php

var_dump(soundex(""));
var_dump(soundex(-1));

$array = array(
"From",
"that",
"time",
"on",
"Sam",
"thought",
"that",
"he",
"sensed",
"a",
"change",
"in",
"Gollum",
"again.",
"He was more fawning and would-be friendly; but Sam surprised some strange looks in his eyes at times, especially towards Frodo."
);

foreach ($array as $str) {
    var_dump(soundex($str));
}

echo "Done\n";
?>
--EXPECT--
string(4) "0000"
string(4) "0000"
string(4) "F650"
string(4) "T300"
string(4) "T500"
string(4) "O500"
string(4) "S500"
string(4) "T230"
string(4) "T300"
string(4) "H000"
string(4) "S523"
string(4) "A000"
string(4) "C520"
string(4) "I500"
string(4) "G450"
string(4) "A250"
string(4) "H256"
Done
