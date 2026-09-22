--TEST--
Testing mb_ereg_search() named capture groups
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_search') or die("skip mb_ereg_search() is not available in this build");
@version_compare(MB_ONIGURUMA_VERSION, '6.9.4', '>=') or die("skip requires oniguruma >= 6.9.4");
?>
--FILE--
<?php
    mb_regex_encoding("UTF-8");
    mb_ereg_search_init('  中国？');
    mb_ereg_search('(?<wsp>\s*)(?<word>\w+)(?<punct>[？！])');
    var_dump(mb_ereg_search_getregs());
?>
--EXPECTF--
Deprecated: Function mb_regex_encoding() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_getregs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
array(7) {
  [0]=>
  string(11) "  中国？"
  [1]=>
  string(2) "  "
  [2]=>
  string(6) "中国"
  [3]=>
  string(3) "？"
  ["wsp"]=>
  string(2) "  "
  ["word"]=>
  string(6) "中国"
  ["punct"]=>
  string(3) "？"
}
