--TEST--
Bug #41445 (parse_ini_file() function parses octal numbers in section names)
--FILE--
<?php

$file = dirname(__FILE__)."/bug41445.ini";

$data = <<<DATA
[001099030277]
option1 = yes

[011099030277]
option2 = yes
DATA;

file_put_contents($file, $data);

var_dump(parse_ini_file($file, TRUE));
var_dump(parse_ini_file($file));

$data = <<<DATA
[23.44]
option1 = yes

[9633337363542736472364]
option2 = yes
DATA;

file_put_contents($file, $data);

var_dump(parse_ini_file($file, TRUE));
var_dump(parse_ini_file($file));

@unlink($file);

echo "Done\n";
?>
--EXPECT--
array(2) {
  [u"001099030277"]=>
  array(1) {
    [u"option1"]=>
    unicode(1) "1"
  }
  [u"011099030277"]=>
  array(1) {
    [u"option2"]=>
    unicode(1) "1"
  }
}
array(2) {
  [u"option1"]=>
  unicode(1) "1"
  [u"option2"]=>
  unicode(1) "1"
}
array(2) {
  [u"23.44"]=>
  array(1) {
    [u"option1"]=>
    unicode(1) "1"
  }
  [u"9633337363542736472364"]=>
  array(1) {
    [u"option2"]=>
    unicode(1) "1"
  }
}
array(2) {
  [u"option1"]=>
  unicode(1) "1"
  [u"option2"]=>
  unicode(1) "1"
}
Done
