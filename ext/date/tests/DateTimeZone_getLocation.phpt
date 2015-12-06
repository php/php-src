--TEST--
DateTimeZone::getLocation -- timezone_location_get — Returns location information for a timezone
public array DateTimeZone::getLocation ( void );
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--SKIPIF--
<?php
if (phpversion() < "5.3.0") {
  die('SKIP php version so lower.');
}
?>
--FILE--
<?php
$arrayDate = DateTimeZone::listAbbreviations();
$countryCode = array("??");

foreach($arrayDate as $value){

    if(NULL != $value[0]['timezone_id']){
        $timeZone = new DateTimeZone($value[0]['timezone_id']);
        $timeZoneArray = $timeZone->getLocation();

        if((!in_array($timeZoneArray['country_code'], $countryCode)) && (NULL != $timeZoneArray['country_code']) && ("" != $timeZoneArray['country_code'])) {
            array_push($countryCode, $timeZoneArray['country_code']);
            print_r($timeZoneArray);
        }
    }
}
?>
--EXPECTF--
Array
(
    [country_code] => AU
    [latitude] => -34.91666
    [longitude] => 138.58333
    [comments] => South Australia
)
Array
(
    [country_code] => CA
    [latitude] => 53.33333
    [longitude] => -60.41666
    [comments] => Atlantic Time - Labrador - most locations
)
Array
(
    [country_code] => ET
    [latitude] => 9.03333
    [longitude] => 38.7
    [comments] => 
)
Array
(
    [country_code] => AF
    [latitude] => 34.51666
    [longitude] => 69.2
    [comments] => 
)
Array
(
    [country_code] => US
    [latitude] => 61.21805
    [longitude] => -149.90027
    [comments] => Alaska Time
)
Array
(
    [country_code] => KZ
    [latitude] => 50.28333
    [longitude] => 57.16666
    [comments] => Aqtobe (Aktobe)
)
Array
(
    [country_code] => AM
    [latitude] => 40.18333
    [longitude] => 44.5
    [comments] => 
)
Array
(
    [country_code] => RU
    [latitude] => 64.75
    [longitude] => 177.48333
    [comments] => Moscow+08 (Moscow+09 after 2014-10-26) - Bering Sea
)
Array
(
    [country_code] => CW
    [latitude] => 12.18333
    [longitude] => -69
    [comments] => 
)
Array
(
    [country_code] => AO
    [latitude] => -8.8
    [longitude] => 13.23333
    [comments] => 
)
Array
(
    [country_code] => SA
    [latitude] => 24.63333
    [longitude] => 46.71666
    [comments] => 
)
Array
(
    [country_code] => PT
    [latitude] => 37.73333
    [longitude] => -25.66666
    [comments] => Azores
)
Array
(
    [country_code] => AZ
    [latitude] => 40.38333
    [longitude] => 49.85
    [comments] => 
)
Array
(
    [country_code] => GB
    [latitude] => 51.50833
    [longitude] => -0.12527
    [comments] => 
)
Array
(
    [country_code] => SO
    [latitude] => 2.06666
    [longitude] => 45.36666
    [comments] => 
)
Array
(
    [country_code] => KE
    [latitude] => -1.28333
    [longitude] => 36.81666
    [comments] => 
)
Array
(
    [country_code] => BB
    [latitude] => 13.1
    [longitude] => -59.61666
    [comments] => 
)
Array
(
    [country_code] => BN
    [latitude] => 4.93333
    [longitude] => 114.91666
    [comments] => 
)
Array
(
    [country_code] => MY
    [latitude] => 1.55
    [longitude] => 110.33333
    [comments] => Sabah & Sarawak
)
Array
(
    [country_code] => BO
    [latitude] => -16.5
    [longitude] => -68.15
    [comments] => 
)
Array
(
    [country_code] => BR
    [latitude] => -23.53333
    [longitude] => -46.61666
    [comments] => S & SE Brazil (GO, DF, MG, ES, RJ, SP, PR, SC, RS)
)
Array
(
    [country_code] => IN
    [latitude] => 22.53333
    [longitude] => 88.36666
    [comments] => 
)
Array
(
    [country_code] => ES
    [latitude] => 28.1
    [longitude] => -15.4
    [comments] => Canary Islands
)
Array
(
    [country_code] => CC
    [latitude] => -12.16666
    [longitude] => 96.91666
    [comments] => 
)
Array
(
    [country_code] => DE
    [latitude] => 52.5
    [longitude] => 13.36666
    [comments] => most locations
)
Array
(
    [country_code] => GL
    [latitude] => 70.48333
    [longitude] => -21.96666
    [comments] => Scoresbysund / Ittoqqortoormiit
)
Array
(
    [country_code] => NZ
    [latitude] => -43.95
    [longitude] => -176.55
    [comments] => Chatham Islands
)
Array
(
    [country_code] => BZ
    [latitude] => 17.5
    [longitude] => -88.2
    [comments] => 
)
Array
(
    [country_code] => MN
    [latitude] => 48.06666
    [longitude] => 114.5
    [comments] => Dornod, Sukhbaatar
)
Array
(
    [country_code] => FM
    [latitude] => 7.41666
    [longitude] => 151.78333
    [comments] => Chuuk (Truk) and Yap
)
Array
(
    [country_code] => CK
    [latitude] => -21.23333
    [longitude] => -159.76666
    [comments] => 
)
Array
(
    [country_code] => CL
    [latitude] => -33.45
    [longitude] => -70.66666
    [comments] => most locations
)
Array
(
    [country_code] => AR
    [latitude] => -34.6
    [longitude] => -58.45
    [comments] => Buenos Aires (BA, CF)
)
Array
(
    [country_code] => CO
    [latitude] => 4.59999
    [longitude] => -74.08333
    [comments] => 
)
Array
(
    [country_code] => UA
    [latitude] => 47.83333
    [longitude] => 35.16666
    [comments] => Zaporozh'ye, E Lugansk / Zaporizhia, E Luhansk
)
Array
(
    [country_code] => CV
    [latitude] => 14.91666
    [longitude] => -23.51666
    [comments] => 
)
Array
(
    [country_code] => CX
    [latitude] => -10.41666
    [longitude] => 105.71666
    [comments] => 
)
Array
(
    [country_code] => GU
    [latitude] => 13.46666
    [longitude] => 144.75
    [comments] => 
)
Array
(
    [country_code] => AQ
    [latitude] => -68.58333
    [longitude] => 77.96666
    [comments] => Davis Station, Vestfold Hills
)
Array
(
    [country_code] => IE
    [latitude] => 53.33333
    [longitude] => -6.25
    [comments] => 
)
Array
(
    [country_code] => TJ
    [latitude] => 38.58333
    [longitude] => 68.8
    [comments] => 
)
Array
(
    [country_code] => SD
    [latitude] => 15.6
    [longitude] => 32.53333
    [comments] => 
)
Array
(
    [country_code] => EC
    [latitude] => -2.16666
    [longitude] => -79.83333
    [comments] => mainland
)
Array
(
    [country_code] => FI
    [latitude] => 60.16666
    [longitude] => 24.96666
    [comments] => 
)
Array
(
    [country_code] => DO
    [latitude] => 18.46666
    [longitude] => -69.9
    [comments] => 
)
Array
(
    [country_code] => MQ
    [latitude] => 14.6
    [longitude] => -61.08333
    [comments] => 
)
Array
(
    [country_code] => FJ
    [latitude] => -18.13333
    [longitude] => 178.41666
    [comments] => 
)
Array
(
    [country_code] => FK
    [latitude] => -51.7
    [longitude] => -57.85
    [comments] => 
)
Array
(
    [country_code] => KG
    [latitude] => 42.9
    [longitude] => 74.59999
    [comments] => 
)
Array
(
    [country_code] => PF
    [latitude] => -23.13333
    [longitude] => -134.94999
    [comments] => Gambier Islands
)
Array
(
    [country_code] => GY
    [latitude] => 6.8
    [longitude] => -58.16666
    [comments] => 
)
Array
(
    [country_code] => GE
    [latitude] => 41.71666
    [longitude] => 44.81666
    [comments] => 
)
Array
(
    [country_code] => GF
    [latitude] => 4.93333
    [longitude] => -52.33333
    [comments] => 
)
Array
(
    [country_code] => GH
    [latitude] => 5.55
    [longitude] => -0.21666
    [comments] => 
)
Array
(
    [country_code] => KI
    [latitude] => 1.41666
    [longitude] => 173
    [comments] => Gilbert Islands
)
Array
(
    [country_code] => CI
    [latitude] => 5.31666
    [longitude] => -4.03333
    [comments] => 
)
Array
(
    [country_code] => AE
    [latitude] => 25.3
    [longitude] => 55.3
    [comments] => 
)
Array
(
    [country_code] => HK
    [latitude] => 22.28333
    [longitude] => 114.15
    [comments] => 
)
Array
(
    [country_code] => CU
    [latitude] => 23.13333
    [longitude] => -82.36666
    [comments] => 
)
Array
(
    [country_code] => TH
    [latitude] => 13.75
    [longitude] => 100.51666
    [comments] => 
)
Array
(
    [country_code] => IL
    [latitude] => 31.78055
    [longitude] => 35.22388
    [comments] => 
)
Array
(
    [country_code] => LK
    [latitude] => 6.93333
    [longitude] => 79.84999
    [comments] => 
)
Array
(
    [country_code] => IO
    [latitude] => -7.33333
    [longitude] => 72.41666
    [comments] => 
)
Array
(
    [country_code] => IR
    [latitude] => 35.66666
    [longitude] => 51.43333
    [comments] => 
)
Array
(
    [country_code] => IS
    [latitude] => 64.15
    [longitude] => -21.85
    [comments] => 
)
Array
(
    [country_code] => ID
    [latitude] => -6.16666
    [longitude] => 106.8
    [comments] => Java & Sumatra
)
Array
(
    [country_code] => KP
    [latitude] => 39.01666
    [longitude] => 125.75
    [comments] => 
)
Array
(
    [country_code] => JP
    [latitude] => 35.65444
    [longitude] => 139.74472
    [comments] => 
)
Array
(
    [country_code] => TW
    [latitude] => 25.05
    [longitude] => 121.5
    [comments] => 
)
Array
(
    [country_code] => PK
    [latitude] => 24.86666
    [longitude] => 67.05
    [comments] => 
)
Array
(
    [country_code] => KR
    [latitude] => 37.54999
    [longitude] => 126.96666
    [comments] => 
)
Array
(
    [country_code] => LT
    [latitude] => 54.68333
    [longitude] => 25.31666
    [comments] => 
)
Array
(
    [country_code] => MH
    [latitude] => 9.08333
    [longitude] => 167.33333
    [comments] => Kwajalein
)
Array
(
    [country_code] => LR
    [latitude] => 6.3
    [longitude] => -10.78333
    [comments] => 
)
Array
(
    [country_code] => LV
    [latitude] => 56.95
    [longitude] => 24.1
    [comments] => 
)
Array
(
    [country_code] => SG
    [latitude] => 1.28333
    [longitude] => 103.85
    [comments] => 
)
Array
(
    [country_code] => MU
    [latitude] => -20.16666
    [longitude] => 57.5
    [comments] => 
)
Array
(
    [country_code] => MV
    [latitude] => 4.16666
    [longitude] => 73.5
    [comments] => 
)
Array
(
    [country_code] => NC
    [latitude] => -22.26666
    [longitude] => 166.44999
    [comments] => 
)
Array
(
    [country_code] => SR
    [latitude] => 5.83333
    [longitude] => -55.16666
    [comments] => 
)
Array
(
    [country_code] => NL
    [latitude] => 52.36666
    [longitude] => 4.9
    [comments] => 
)
Array
(
    [country_code] => NF
    [latitude] => -29.05
    [longitude] => 167.96666
    [comments] => 
)
Array
(
    [country_code] => NR
    [latitude] => -0.51666
    [longitude] => 166.91666
    [comments] => 
)
Array
(
    [country_code] => NU
    [latitude] => -19.01666
    [longitude] => -169.91666
    [comments] => 
)
Array
(
    [country_code] => PE
    [latitude] => -12.05
    [longitude] => -77.05
    [comments] => 
)
Array
(
    [country_code] => PG
    [latitude] => -9.5
    [longitude] => 147.16666
    [comments] => most locations
)
Array
(
    [country_code] => PH
    [latitude] => 14.58333
    [longitude] => 121
    [comments] => 
)
Array
(
    [country_code] => PM
    [latitude] => 47.05
    [longitude] => -56.33333
    [comments] => 
)
Array
(
    [country_code] => PN
    [latitude] => -25.06666
    [longitude] => -130.08333
    [comments] => 
)
Array
(
    [country_code] => HT
    [latitude] => 18.53333
    [longitude] => -72.33333
    [comments] => 
)
Array
(
    [country_code] => PY
    [latitude] => -25.26666
    [longitude] => -57.66666
    [comments] => 
)
Array
(
    [country_code] => RE
    [latitude] => -20.86666
    [longitude] => 55.46666
    [comments] => 
)
Array
(
    [country_code] => UZ
    [latitude] => 39.66666
    [longitude] => 66.8
    [comments] => west Uzbekistan
)
Array
(
    [country_code] => ZA
    [latitude] => -26.25
    [longitude] => 28
    [comments] => 
)
Array
(
    [country_code] => SB
    [latitude] => -9.53333
    [longitude] => 160.19999
    [comments] => 
)
Array
(
    [country_code] => SC
    [latitude] => -4.66666
    [longitude] => 55.46666
    [comments] => 
)
Array
(
    [country_code] => WS
    [latitude] => -13.83333
    [longitude] => -171.73333
    [comments] => 
)
Array
(
    [country_code] => CR
    [latitude] => 9.93333
    [longitude] => -84.08333
    [comments] => 
)
Array
(
    [country_code] => NA
    [latitude] => -22.56666
    [longitude] => 17.1
    [comments] => 
)
Array
(
    [country_code] => TF
    [latitude] => -49.35277
    [longitude] => 70.2175
    [comments] => 
)
Array
(
    [country_code] => TK
    [latitude] => -9.36666
    [longitude] => -171.23333
    [comments] => 
)
Array
(
    [country_code] => TL
    [latitude] => -8.55
    [longitude] => 125.58333
    [comments] => 
)
Array
(
    [country_code] => TO
    [latitude] => -21.16666
    [longitude] => -175.16666
    [comments] => 
)
Array
(
    [country_code] => TR
    [latitude] => 41.01666
    [longitude] => 28.96666
    [comments] => 
)
Array
(
    [country_code] => TV
    [latitude] => -8.51666
    [longitude] => 179.21666
    [comments] => 
)
Array
(
    [country_code] => UY
    [latitude] => -34.88333
    [longitude] => -56.18333
    [comments] => 
)
Array
(
    [country_code] => VE
    [latitude] => 10.5
    [longitude] => -66.93333
    [comments] => 
)
Array
(
    [country_code] => VU
    [latitude] => -17.66666
    [longitude] => 168.41666
    [comments] => 
)
Array
(
    [country_code] => UM
    [latitude] => 19.28333
    [longitude] => 166.61666
    [comments] => Wake Island
)
Array
(
    [country_code] => CG
    [latitude] => -4.26666
    [longitude] => 15.28333
    [comments] => 
)
Array
(
    [country_code] => FR
    [latitude] => 48.86666
    [longitude] => 2.33333
    [comments] => 
)
Array
(
    [country_code] => WF
    [latitude] => -13.3
    [longitude] => -176.16666
    [comments] => 
)
