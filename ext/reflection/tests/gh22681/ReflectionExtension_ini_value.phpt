--TEST--
GH-22681: null bytes in INI values truncate ReflectionExtension::__toString()
--FILE--
<?php

ini_set('arg_separator.output', "f\0oo");
$r = new ReflectionExtension('core');
$str = (string)$r;
$index = strpos($str, 'Entry [ arg_separator.output');
$str = substr($str, $index);
$index = strpos($str, 'Entry', 1);
$str = substr($str, 0, $index);
echo $str . "\n";
var_dump( $r->getINIEntries()['arg_separator.output'] );

?>
--EXPECTF--
Entry [ arg_separator.output <ALL> ]
      Current = 'f'
      Default = '&'
    }
    
string(4) "f%0oo"
