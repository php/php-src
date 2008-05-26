--TEST--
Bug #35239 (Objects can lose references)
--FILE--
<?php
$a = new stdClass; 
$a->x0 = new stdClass;
$a->x0->y0 = 'a';
$a->x0->y1 =& $a->x0;
$a->x0->y2 =& $a->x0;
$a->x0->y0 = 'b';
var_dump($a);
$a->x0->y1 = "ok\n";
echo $a->x0;
?>
--EXPECT--
object(stdClass)#1 (1) {
  [u"x0"]=>
  &object(stdClass)#2 (3) {
    [u"y0"]=>
    unicode(1) "b"
    [u"y1"]=>
    &object(stdClass)#2 (3) {
      [u"y0"]=>
      unicode(1) "b"
      [u"y1"]=>
      *RECURSION*
      [u"y2"]=>
      *RECURSION*
    }
    [u"y2"]=>
    &object(stdClass)#2 (3) {
      [u"y0"]=>
      unicode(1) "b"
      [u"y1"]=>
      *RECURSION*
      [u"y2"]=>
      *RECURSION*
    }
  }
}
ok
