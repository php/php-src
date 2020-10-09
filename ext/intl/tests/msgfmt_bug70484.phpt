--TEST--
Bug #70484 selectordinal doesn't work with named parameters
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php

$locale = array("de", "fr", "en", "ru",);

$data = array(42, 42.42, 2147483643, 2147483643.12345, 5);

foreach ($locale as $lc) {
    echo "$lc string key\n";
    $m = new MessageFormatter($lc, "{n, selectordinal, =5 {five} zero {#-zero} one {#-one} two {#-two} few {#-few} many {#-many} other {#-other}}");
    foreach ($data as $i) {
        var_dump($m->format(array("n" => $i)));
        if ($m->getErrorCode()) {
            echo "$lc $i ", $m->getErrorMessage();
        }
    }
    echo "\n";

    echo "$lc numeric key\n";
    $m = new MessageFormatter($lc, "{0, selectordinal, =5 {five} zero {#-zero} one {#-one} two {#-two} few {#-few} many {#-many} other {#-other}}");
    foreach ($data as $i) {
        var_dump($m->format(array($i)));
        if ($m->getErrorCode()) {
            echo "$lc $i ", $m->getErrorMessage();
        }
    }
    echo "\n";
}

?>
--EXPECTF--
de string key
string(8) "42-other"
string(11) "42,42-other"
string(19) "2.147.483.643-other"
string(23) "2.147.483.643,123-other"
string(4) "five"

de numeric key
string(8) "42-other"
string(11) "42,42-other"
string(19) "2.147.483.643-other"
string(23) "2.147.483.643,123-other"
string(4) "five"

fr string key
string(8) "42-other"
string(11) "42,42-other"
string(%d) "2%s147%s483%s643-other"
string(%d) "2%s147%s483%s643,123-other"
string(4) "five"

fr numeric key
string(8) "42-other"
string(11) "42,42-other"
string(%d) "2%s147%s483%s643-other"
string(%d) "2%s147%s483%s643,123-other"
string(4) "five"

en string key
string(6) "42-two"
string(11) "42.42-other"
string(17) "2,147,483,643-few"
string(23) "2,147,483,643.123-other"
string(4) "five"

en numeric key
string(6) "42-two"
string(11) "42.42-other"
string(17) "2,147,483,643-few"
string(23) "2,147,483,643.123-other"
string(4) "five"

ru string key
string(8) "42-other"
string(11) "42,42-other"
string(22) "2 147 483 643-other"
string(26) "2 147 483 643,123-other"
string(4) "five"

ru numeric key
string(8) "42-other"
string(11) "42,42-other"
string(22) "2 147 483 643-other"
string(26) "2 147 483 643,123-other"
string(4) "five"

