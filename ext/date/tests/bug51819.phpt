--TEST--
Bug #51819 (Case discrepancy in timezone names cause Uncaught exception and fatal error)
--FILE--
<?php
date_default_timezone_set('UTC');

$aTzAbbr = timezone_abbreviations_list();

$aTz = array();
foreach (array_keys($aTzAbbr) as $sKey) {
    foreach (array_keys($aTzAbbr[$sKey]) as $iIndex) {
        $sTz = $aTzAbbr[$sKey][$iIndex]['timezone_id'];

        if (! in_array($sTz, $aTz)) {
            array_push($aTz, $sTz);
        }
    }
}

foreach ($aTz as $sTz) {
    $sDate = '2010-05-15 00:00:00 ' . $sTz;

    try {
        $oDateTime = new DateTime($sDate);
    } catch (Exception $oException) {
        var_dump($oException->getMessage());
        print_r(DateTime::getLastErrors());
    }
}

var_dump('this should be the only output');
?>
--EXPECT--
string(30) "this should be the only output"
