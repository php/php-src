--TEST--
Bug #69485 (Double free on zend_list_dtor)
--EXTENSIONS--
curl
--FILE--
<?php

class O {
    public $ch;
    public function dummy() {
    }
}

$ch = curl_init();

$o = new O;
$o->ch = $ch;
curl_setopt($ch, CURLOPT_WRITEFUNCTION, array($o, "dummy"));
?>
==DONE==
--EXPECT--
==DONE==
