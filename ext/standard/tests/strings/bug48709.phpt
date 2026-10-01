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
--EXPECTF--
Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
kn => N

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
gn => N

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
pn => N

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
ae => E

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
wr => R

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
x => S

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
wh => W

Deprecated: Function metaphone() is deprecated since 8.6, use a userland phonetic matching library instead in %s on line %d
wa => W
