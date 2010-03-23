--TEST--
Bug #33200 (magic_quotes_sybase = On makes 'e' modifier misbehave)
--INI--
magic_quotes_sybase=1
--FILE--
<?php
$str = 'some \'$sample\' text';
$str = preg_replace("/(some.*text)/e", "strtoupper('\\1')", $str);
echo $str . "\r\n";
?>
--EXPECT--
Warning: Directive 'magic_quotes_sybase' is deprecated in PHP 5.3 and greater in Unknown on line 0
SOME '$SAMPLE' TEXT
