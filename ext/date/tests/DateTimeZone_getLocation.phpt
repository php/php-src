--TEST--
DateTimeZone::getLocation -- timezone_location_get — Returns location information for a timezone public array DateTimeZone::getLocation ( void ) ;
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br> - #phparty7 - @phpsp - novatec/2015 - sao paulo - br
--FILE--
<?php
$arrayDate = DateTimeZone::listAbbreviations();
$countryCode = array("??");
$countryCodeTest = array("AU", "CA", "ET", "AF", "US", "KZ", "AM");

foreach($arrayDate as $value){

    if(NULL != $value[0]['timezone_id']){
        $timeZone = new DateTimeZone($value[0]['timezone_id']);
        $timeZoneArray = $timeZone->getLocation();

        if((!in_array($timeZoneArray['country_code'], $countryCode)) && (NULL != $timeZoneArray['country_code']) && ("" != $timeZoneArray['country_code'])) {
            array_push($countryCode, $timeZoneArray['country_code']);

            if(in_array($timeZoneArray['country_code'], $countryCodeTest)){
                print_r($timeZoneArray);
            }
        }
    }
}
?>
--CLEAN--
<?php
unset($arrayDate);
unset($countryCode);
unset($countryCodeTest);
?>
--EXPECTF--
Array
(
    [country_code] => %s
    [latitude] => %f
    [longitude] => %f
    [comments] => %s
)
Array
(
    [country_code] => %s
    [latitude] => %f
    [longitude] => %f
    [comments] => %s
)
Array
(
    [country_code] => %s
    [latitude] => %f
    [longitude] => %f
    [comments] => %s
)
