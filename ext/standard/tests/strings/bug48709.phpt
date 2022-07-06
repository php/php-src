--TEST--
Bug #48709 (metaphone and 'wh')
--FILE--
<?php

/* Initial letter exceptions */
$exceptions = array(
    'kn', // Drop first letter
    'gn', // ditto
    'pn', // ditto
    'ae', // ditto
    'wr', // ditto
    'x',  // s
    'wh', // w
    'wa'  // w
);

foreach ($exceptions as $letter) {
    printf("%s => %s\n", $letter, metaphone($letter));
}

?>
--EXPECT--
kn => N
gn => N
pn => N
ae => E
wr => R
x => S
wh => W
wa => W
