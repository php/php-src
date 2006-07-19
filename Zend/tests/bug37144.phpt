--TEST--
Bug #37144 (PHP crashes trying to assign into property of dead object)
--FILE--
<?php
function foo() {
  $x = new stdClass();
  $x->bar = array(1);
  return $x;
}
foo()->bar[1] = "123";
foo()->bar[0]++;
unset(foo()->bar[0]);
echo "ok\n";
?>
--EXPECT--
ok
