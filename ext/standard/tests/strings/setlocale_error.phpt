--TEST--
Test setlocale() function : error condition
--INI--
error_reporting=E_ALL
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip Not valid for windows');
}
?>
--FILE--
<?php
echo "*** Testing setlocale() : error conditions ***\n";

echo "\n-- Testing setlocale() function with invalid locale array, 'category' = LC_ALL --\n";
//Invalid array of locales
$invalid_locales = array("en_US.invalid", "en_AU.invalid", "ko_KR.invalid");
try {
    var_dump(setlocale(LC_ALL,$invalid_locales));
} catch (\Error $e) {
    var_dump($e->getMessage());
}

echo "\n-- Testing setlocale() function with invalid multiple locales, 'category' = LC_ALL --\n";
//Invalid array of locales
try {
    var_dump(setlocale(LC_ALL,"en_US.invalid", "en_AU.invalid", "ko_KR.invalid"));
} catch (\Error $e) {
    var_dump($e->getMessage());
}

echo "\n-- Testing setlocale() function with locale name too long, 'category' = LC_ALL --";
//Invalid locale - locale name too long
try {
    var_dump(setlocale(LC_ALL,str_pad('',255,'A')));
} catch (\Error $e) {
    var_dump($e->getMessage());
}

echo "\nDone";
?>
--EXPECTF--
*** Testing setlocale() : error conditions ***

-- Testing setlocale() function with invalid locale array, 'category' = LC_ALL --
string(48) "Requested locale is not installed on your system"

-- Testing setlocale() function with invalid multiple locales, 'category' = LC_ALL --
string(48) "Requested locale is not installed on your system"

-- Testing setlocale() function with locale name too long, 'category' = LC_ALL --
Warning: setlocale(): Specified locale name is too long in %s on line %d
string(48) "Requested locale is not installed on your system"

Done
