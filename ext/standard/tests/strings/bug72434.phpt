--TEST--
Bug #72434: ZipArchive class Use After Free Vulnerability in PHP's GC algorithm and unserialize
--SKIPIF--
<?php
if(!class_exists('zip')) die('ZipArchive');
?>
--FILE--
<?php
// The following array will be serialized and this representation will be freed later on.
$free_me = array(new StdClass());
// Create our payload and unserialize it.
$serialized_payload = 'a:3:{i:1;N;i:2;O:10:"ZipArchive":1:{s:8:"filename";'.serialize($free_me).'}i:1;R:4;}';
$unserialized_payload = unserialize($serialized_payload);
gc_collect_cycles();
// The reference counter for $free_me is at -1 for PHP 7 right now.
// Increment the reference counter by 1 -> rc is 0
$a = $unserialized_payload[1];
// Increment the reference counter by 1 again -> rc is 1
$b = $a;
// Trigger free of $free_me (referenced by $m[1]).
unset($b);
$fill_freed_space_1 = "filler_zval_1";
$fill_freed_space_2 = "filler_zval_2";
$fill_freed_space_3 = "filler_zval_3";
$fill_freed_space_4 = "filler_zval_4";
debug_zval_dump($unserialized_payload[1]);
?>
--EXPECTF--
array(1) refcount(3){
  [0]=>
  object(stdClass)#%d (0) refcount(1){
  }
}
