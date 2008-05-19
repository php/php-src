--TEST--
Bug #36611 (assignment to SimpleXML object attribute changes argument type to string)
--SKIPIF--
<?php if (!extension_loaded("simplexml")) print "skip"; ?>
--FILE--
<?php

$xml_str = b<<<EOD
<?xml version="1.0" encoding="ISO-8859-1" ?>
<c_fpobel >
  <pos >
    <pos/>
  </pos>
</c_fpobel>
EOD;

$xml = simplexml_load_string ($xml_str) ;

$val = 1;

var_dump($val);
$obj->pos["act_idx"] = $val;
var_dump($val) ;

echo "Done\n";
?>
--EXPECTF--	
int(1)
int(1)
Strict Standards: Creating default object from empty value in %s on line %d
Done
--UEXPECTF--
int(1)

Strict Standards: Creating default object from empty value in %s on line %d
int(1)
Done
