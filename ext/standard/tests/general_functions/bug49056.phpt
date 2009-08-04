--TEST--
Bug #49056 (parse_ini_*() regression in 5.3.0 when using non-ASCII strings as option keys)
--FILE--
<?php

$string = <<<EOT
Cooking_furniture="KÃ¼chen MÃ¶bel (en)"
KÃ¼chen_MÃ¶bel="Cooking furniture (en)"
EOT;

$filename = dirname(__FILE__) . '/bug49056.tmp';

file_put_contents( $filename, $string);

var_dump(parse_ini_file($filename));

?>
--CLEAN--
<?php @unlink(dirname(__FILE__) . '/bug49056.tmp'); ?>
--EXPECT--
array(2) {
  [u"Cooking_furniture"]=>
  unicode(19) "KÃ¼chen MÃ¶bel (en)"
  [u"KÃ¼chen_MÃ¶bel"]=>
  unicode(22) "Cooking furniture (en)"
}
