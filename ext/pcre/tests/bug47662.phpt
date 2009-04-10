--TEST--
Bug #47662 (support more than 127 named subpatterns)
--FILE--
<?php

$regex = '@';
for($bar=0; $bar<1027; $bar++) {
	$regex .= '((?P<x'.$bar.'>))';
}
$regex .= 'fo+bar@';

var_dump(preg_match($regex, 'foobar'));
echo "Done!\n";

?>
--EXPECT--
int(1)
Done!
