--TEST--
metaphone() tests
--FILE--
<?php

var_dump(metaphone(""));
var_dump(metaphone(-1));

try {
    var_dump(metaphone("valid phrase", -1));
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), "\n";
}
var_dump(metaphone("valid phrase", 0));
var_dump(metaphone("valid phrase", 10000));

$array = array(
"They fell forward, grovelling heedlessly on the cold earth.",
"But the shadow of horror wheeled and returned, passing lower now, right above them, sweeping the fen-reek with its ghastly wings.",
"And then it was gone, flying back to Mordor with the speed of the wrath of Sauron; and behind it the wind roared away, leaving the Dead Marshes bare and bleak.",
"The naked waste, as far as the eye could pierce, even to the distant menace of the mountains, was dappled with the fitful moonlight."
);

foreach($array as $str) {
    var_dump(metaphone($str));
}

echo "Done\n";
?>
--EXPECTF--
Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(0) ""

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(0) ""

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
ValueError: metaphone(): Argument #2 ($max_phonemes) must be greater than or equal to 0

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(6) "FLTFRS"

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(6) "FLTFRS"

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(26) "0FLFRWRTKRFLNKHTLSLN0KLTR0"

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(56) "BT0XTFHRRHLTNTRTRNTPSNKLWRNRFTBF0MSWPNK0FNRKW0TSFSTLWNKS"

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(69) "ANT0NTWSKNFLYNKBKTMRTRW00SPTF0R0FSRNNTBHNTT0WNTRRTWLFNK0TTMRXSBRNTBLK"

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
string(56) "0NKTWSTSFRS0YKLTPRSFNT0TSTNTMNSF0MNTNSWSTPLTW00FTFLMNLFT"
Done
