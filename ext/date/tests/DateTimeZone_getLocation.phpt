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
    [latitude] => %f
    [longitude] => %f
    [comments] => South Australia
)
Array
(
    [country_code] => CA
    [latitude] => %f
    [longitude] => %f
    [comments] => Atlantic Time - Labrador - most locations
)
Array
(
    [country_code] => ET
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => AF
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => US
    [latitude] => %f
    [longitude] => %f
    [comments] => Alaska Time
)
Array
(
    [country_code] => KZ
    [latitude] => %f
    [longitude] => %f
    [comments] => Aqtobe (Aktobe)
)
Array
(
    [country_code] => AM
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => RU
    [latitude] => %f
    [longitude] => %f
    [comments] => Moscow+08 (Moscow+09 after 2014-10-26) - Bering Sea
)
Array
(
    [country_code] => CW
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => AO
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => SA
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => PT
    [latitude] => %f
    [longitude] => %f
    [comments] => Azores
)
Array
(
    [country_code] => AZ
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => GB
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => SO
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => KE
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => BB
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => BN
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => MY
    [latitude] => %f
    [longitude] => %f
    [comments] => Sabah & Sarawak
)
Array
(
    [country_code] => BO
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => BR
    [latitude] => %f
    [longitude] => %f
    [comments] => S & SE Brazil (GO, DF, MG, ES, RJ, SP, PR, SC, RS)
)
Array
(
    [country_code] => IN
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => ES
    [latitude] => %f
    [longitude] => %f
    [comments] => Canary Islands
)
Array
(
    [country_code] => CC
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => DE
    [latitude] => %f
    [longitude] => %f
    [comments] => most locations
)
Array
(
    [country_code] => GL
    [latitude] => %f
    [longitude] => %f
    [comments] => Scoresbysund / Ittoqqortoormiit
)
Array
(
    [country_code] => NZ
    [latitude] => %f
    [longitude] => %f
    [comments] => Chatham Islands
)
Array
(
    [country_code] => BZ
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => MN
    [latitude] => %f
    [longitude] => %f
    [comments] => Dornod, Sukhbaatar
)
Array
(
    [country_code] => FM
    [latitude] => %f
    [longitude] => %f
    [comments] => Chuuk (Truk) and Yap
)
Array
(
    [country_code] => CK
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => CL
    [latitude] => %f
    [longitude] => %f
    [comments] => most locations
)
Array
(
    [country_code] => AR
    [latitude] => %f
    [longitude] => %f
    [comments] => Buenos Aires (BA, CF)
)
Array
(
    [country_code] => CO
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => UA
    [latitude] => %f
    [longitude] => %f
    [comments] => Zaporozh'ye, E Lugansk / Zaporizhia, E Luhansk
)
Array
(
    [country_code] => CV
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => CX
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => GU
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => AQ
    [latitude] => %f
    [longitude] => %f
    [comments] => Davis Station, Vestfold Hills
)
Array
(
    [country_code] => IE
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => TJ
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => SD
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => EC
    [latitude] => %f
    [longitude] => %f
    [comments] => mainland
)
Array
(
    [country_code] => FI
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => DO
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => MQ
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => FJ
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => FK
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => KG
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => PF
    [latitude] => %f
    [longitude] => %f
    [comments] => Gambier Islands
)
Array
(
    [country_code] => GY
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => GE
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => GF
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => GH
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => KI
    [latitude] => %f
    [longitude] => %f
    [comments] => Gilbert Islands
)
Array
(
    [country_code] => CI
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => AE
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => HK
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => CU
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => TH
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => IL
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => LK
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => IO
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => IR
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => IS
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => ID
    [latitude] => %f
    [longitude] => %f
    [comments] => Java & Sumatra
)
Array
(
    [country_code] => KP
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => JP
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => TW
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => PK
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => KR
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => LT
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => MH
    [latitude] => %f
    [longitude] => %f
    [comments] => Kwajalein
)
Array
(
    [country_code] => LR
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => LV
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => SG
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => MU
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => MV
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => NC
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => SR
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => NL
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => NF
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => NR
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => NU
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => PE
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => PG
    [latitude] => %f
    [longitude] => %f
    [comments] => most locations
)
Array
(
    [country_code] => PH
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => PM
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => PN
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => HT
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => PY
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => RE
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => UZ
    [latitude] => %f
    [longitude] => %f
    [comments] => west Uzbekistan
)
Array
(
    [country_code] => ZA
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => SB
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => SC
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => WS
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => CR
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => NA
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => TF
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => TK
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => TL
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => TO
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => TR
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => TV
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => UY
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => VE
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => VU
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => UM
    [latitude] => %f
    [longitude] => %f
    [comments] => Wake Island
)
Array
(
    [country_code] => CG
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => FR
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
Array
(
    [country_code] => WF
    [latitude] => %f
    [longitude] => %f
    [comments] => 
)
