--TEST--
Bug #34873 (Segmentation Fault on foreach in object)
--FILE--
<?php
class pwa {
    public $var;

    function __construct(){
        $this->var = array();
    }

    function test (){
        $cont = array();
        $cont["mykey"] = "myvalue";

        foreach ($cont as $this->var['key'] => $this->var['value'])
        var_dump($this->var['value']);
    }
}
$myPwa = new Pwa();
$myPwa->test();

echo "Done\n";
?>
--EXPECT--
string(7) "myvalue"
Done
