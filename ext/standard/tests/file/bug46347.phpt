--TEST--
Bug #46347 (parse_ini_file() doesn't support * in keys)
--FILE--
<?php

$str = <<< EOF
[section]
part1.*.part2 = 1
EOF;

$file = __DIR__ . '/parse.ini';
file_put_contents($file, $str);

var_dump(parse_ini_file($file));
?>
--CLEAN--
<?php
unlink(__DIR__.'/parse.ini');
?>
--EXPECTF--
array(1) {
  [%u|b%"part1.*.part2"]=>
  %unicode|string%(1) "1"
}
