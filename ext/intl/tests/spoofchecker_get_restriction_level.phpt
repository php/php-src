--TEST--
Spoofchecker::getRestrictionLevel() returns the configured restriction level
--EXTENSIONS--
intl
--FILE--
<?php
$checker = new Spoofchecker();
var_dump($checker->getRestrictionLevel() === Spoofchecker::HIGHLY_RESTRICTIVE);

foreach ([
    Spoofchecker::ASCII,
    Spoofchecker::SINGLE_SCRIPT_RESTRICTIVE,
    Spoofchecker::HIGHLY_RESTRICTIVE,
    Spoofchecker::MODERATELY_RESTRICTIVE,
    Spoofchecker::MINIMALLY_RESTRICTIVE,
    Spoofchecker::UNRESTRICTIVE,
] as $level) {
    $checker->setRestrictionLevel($level);
    var_dump($checker->getRestrictionLevel() === $level);
}

// Disabling checks does not change the configured restriction level.
$checker->setRestrictionLevel(Spoofchecker::ASCII);
$checker->setChecks(0);
var_dump($checker->getRestrictionLevel() === Spoofchecker::ASCII);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
