--TEST--
SpoofChecker with self references
--EXTENSIONS--
intl
--FILE--
<?php

$checker = new Spoofchecker();
$checker->isSuspicious("", $checker);

$checker = new Spoofchecker();
$checker->areConfusable("", "", $checker);

if (version_compare(INTL_ICU_VERSION, '74.0') >= 0) {
    $checker = new Spoofchecker();
    $checker->areBidiConfusable(Spoofchecker::LTR, "", "", $checker);
}

echo "Done\n";

?>
--EXPECT--
Done
