--TEST--
Spoofchecker::getRestrictionLevel() rejects an unconstructed object
--EXTENSIONS--
intl
--FILE--
<?php
$checker = new class extends Spoofchecker {
    public function __construct() {}
};

try {
    $checker->getRestrictionLevel();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
Found unconstructed Spoofchecker
