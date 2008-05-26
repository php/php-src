--TEST--
Bug #36975 (natcasesort() causes array_pop() to misbehave)
--FILE--
<?php
$a = array('aa', 'aa', 'bb', 'bb', 'cc', 'cc');                  
$test = natcasesort($a);
if ($test) {                                                            
  echo "natcasesort success!\n";                                        
}                                                                       
$val = array_pop($a);                                            
$a[] = $val;                                                     
var_dump($a);

$b = array(1 => 'foo', 0 => 'baz');
array_pop($b);
$b[] = 'bar';
array_push($b, 'bar');
print_r($b);

$c = array(0, 0, 0, 0, 0);
asort($c);
array_pop($c);
$c[] = 'foo';
$c[] = 'bar';
var_dump($c);
?>
--EXPECT--
natcasesort success!
array(6) {
  [0]=>
  unicode(2) "aa"
  [1]=>
  unicode(2) "aa"
  [3]=>
  unicode(2) "bb"
  [2]=>
  unicode(2) "bb"
  [5]=>
  unicode(2) "cc"
  [6]=>
  unicode(2) "cc"
}
Array
(
    [1] => foo
    [2] => bar
    [3] => bar
)
array(6) {
  [4]=>
  int(0)
  [3]=>
  int(0)
  [2]=>
  int(0)
  [1]=>
  int(0)
  [5]=>
  unicode(3) "foo"
  [6]=>
  unicode(3) "bar"
}
