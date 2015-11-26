--TEST--
Funktionstest mb_ereg_search_regs()
--SKIPIF--
<?php
if (!extension_loaded('mbstring')) die('skip mbstring not enabled');
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
--EXPECT--
array(1) {
  [0]=>
  string(4) "ов"
}
