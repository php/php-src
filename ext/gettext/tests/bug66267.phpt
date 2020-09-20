--TEST--
#66265: gettext doesn't switch locales within the same script
--SKIPIF--
<?php
if (!extension_loaded("gettext")) {
    die("skip\n");
}
if (PHP_ZTS) {
    /* this is supposed to fail on the TS build at least on Windows,
    should be even XFAIL till it's fixed there */
    die("skip NTS only");
}
if (substr(PHP_OS, 0, 3) != 'WIN') {
    $loc = ["de_DE", "fr_FR", "en_US"];
    foreach($loc as $l) {
        if (!setlocale(LC_ALL, $l)) {
            die("SKIP '$l' locale not supported.");
        }
    }
}
?>
--FILE--
<?php

$domain = 'domain';

$loc = ["de_DE", "fr_FR", "en_US"];

foreach ($loc as $l) {
    putenv("LC_ALL=$l");
    setlocale(LC_ALL, $l);

    $path = realpath(__DIR__ . DIRECTORY_SEPARATOR . "66265");
    bindtextdomain($domain, $path);
    bind_textdomain_codeset($domain, "UTF-8");
    textdomain($domain);

    echo 'LC_ALL=', getenv('LC_ALL'), "\n";
    echo 'hello=', _('hello'), "\n";
    echo "\n";
}

?>
--EXPECT--
LC_ALL=de_DE
hello=hallo

LC_ALL=fr_FR
hello=salut

LC_ALL=en_US
hello=hello

