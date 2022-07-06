--TEST--
Bug #73746 (Method that returns string returns UNKNOWN:0 instead)
--SKIPIF--
<?php require_once('skipif.inc'); ?>
--FILE--
<?php
namespace Core\Bundle\Service\Property\Room\Rooms;

class CountryMapping
{
    const CZ = 'CZ';
    const EN = 'EN';

    public function get(string $countryIsoCode = null) : string // Works correctly if return type is removed
    {
        switch (strtoupper($countryIsoCode)) {
        case 'CZ':
        case 'SK':
            return self::CZ; // Works correctly if changed to CountryMapping::CZ
        default:
            return self::EN; // Works correctly if changed to CountryMapping::EN
        }
    }
}

$mapping = new CountryMapping();
var_dump($mapping->get('CZ'));
?>
--EXPECT--
string(2) "CZ"
