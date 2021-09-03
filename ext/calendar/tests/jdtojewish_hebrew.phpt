--TEST--
Test all hebrew month names
--EXTENSIONS--
calendar
--FILE--
<?php
for ($year = 5000; $year <= 5001; $year++) {
    $leap = ($year === 5001) ? 'leap' : 'normal';
    echo "$leap year $year\n";
    for ($month = 1; $month <= 13; $month++) {
        $jd = jewishtojd($month, 1, $year);
        $hebrew = jdtojewish($jd, true);
        $hex = bin2hex($hebrew);
        echo "$hex\n";
    }
    echo "\n";
}
?>
--EXPECT--
normal year 5000
e020faf9f8e920e4
e020e7f9e5ef20e4
e020ebf1ece520e4
e020e8e1fa20e4
e020f9e1e820e4
e020e0e3f820e02720e4
e020e0e3f820e12720e4
e020f0e9f1ef20e4
e020e0e9e9f820e4
e020f1e9e5ef20e4
e020faeee5e620e4
e020e0e120e4
e020e0ece5ec20e4

leap year 5001
e020faf9f8e920e4e0
e020e7f9e5ef20e4e0
e020ebf1ece520e4e0
e020e8e1fa20e4e0
e020f9e1e820e4e0
e020e0e3f820e4e0
e020e0e3f820e4e0
e020f0e9f1ef20e4e0
e020e0e9e9f820e4e0
e020f1e9e5ef20e4e0
e020faeee5e620e4e0
e020e0e120e4e0
e020e0ece5ec20e4e0

