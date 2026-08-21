--TEST--
GH-23332: Saturation of ArrayObject iterator leads to UAF
--FILE--
<?php
$owner = new ArrayObject(['entry' => 1337]);

$iterators = [];
for ($i = 0; $i < 255; $i++) {
  $it = $owner->getIterator();
  $it->rewind();
  $iterators[] = $it;
}
unset($it);

$array = (array) $owner;

$pass = 0;
$retained = null;
foreach ($array as &$value) {
  if (++$pass === 1) {
    $retained = $array;
    $array = ['replacement' => 4242];
    continue;
  }
  for ($i = 0; $i < 254; $i++) {
    unset($iterators[$i]);
  }
  $retained = null;
  unset($iterators[254]);
}

echo "done\n";
var_dump($pass);
?>
--EXPECT--
done
int(2)
