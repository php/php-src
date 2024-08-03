--TEST--
GH-14930: Custom stream wrapper dir_readdir output truncated to 255 characters in PHP 8.3
--FILE--
<?php

class DummyWrapper
{
    public $context;

    public function dir_opendir($url, $options)
    {
        return true;
    }

    public function dir_readdir()
    {
        return 'very-long-filename-ieNoquiaC6ijeiy9beejaiphoriejo2cheehooGou8uhoh7eh0gefahyuQuohd7eec9auso9eeFah2Maedohsemi1eetoo5fo5biePh5eephai7SiuguipouLeemequ2oope9aigoQu5efak2aLeri9ithaiJ9eew3dianaiHoo1aexaighiitee6geghiequ5nohhiikahwee8ohk2Soip2Aikeithohdeitiedeiku7DiTh2eep3deitiedeiku7DiTh2ee.txt';
    }
}

stream_wrapper_register('dummy', DummyWrapper::class);

$dh = opendir('dummy://', stream_context_create());
var_dump(readdir($dh));
?>
--EXPECT--
string(288) "very-long-filename-ieNoquiaC6ijeiy9beejaiphoriejo2cheehooGou8uhoh7eh0gefahyuQuohd7eec9auso9eeFah2Maedohsemi1eetoo5fo5biePh5eephai7SiuguipouLeemequ2oope9aigoQu5efak2aLeri9ithaiJ9eew3dianaiHoo1aexaighiitee6geghiequ5nohhiikahwee8ohk2Soip2Aikeithohdeitiedeiku7DiTh2eep3deitiedeiku7DiTh2ee.txt"
