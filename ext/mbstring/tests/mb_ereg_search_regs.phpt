--TEST--
Funktionstest mb_ereg_search_regs()
--EXTENSIONS--
mbstring
--SKIPIF--
<?php
function_exists('mb_ereg_search_regs') or die("skip mb_ereg_search_regs() not available");
?>
--FILE--
<?php
    // homepage:

    //$mb_str = "Алексей Федорович Карамазов был Алексей Федорович Карамазов был kyrillischer string string";
    //      = "Lorem ipsum dolor sit amet"

    mb_ereg_search_init("Алексей Федорович Карамазов был Алексей Федорович Карамазов был");



    $match= mb_ereg_search_regs("ов");
    var_dump($match);


?>
--EXPECTF--
Deprecated: Function mb_ereg_search_init() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d

Deprecated: Function mb_ereg_search_regs() is deprecated since 8.6, because the underlying library is no longer maintained in %s on line %d
array(1) {
  [0]=>
  string(4) "ов"
}
