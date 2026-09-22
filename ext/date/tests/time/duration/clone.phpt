--TEST--
Time\Duration: clone
--FILE--
<?php

require __DIR__ . '/helper.inc';

function fc(Time\Duration $d): string {
	return f(clone($d));
}

echo fc(Time\Duration::fromSeconds(0, 0)), PHP_EOL;
echo fc(Time\Duration::fromSeconds(0, 1)), PHP_EOL;
echo fc(Time\Duration::fromSeconds(1, 1)), PHP_EOL;
echo fc(Time\Duration::fromSeconds(0, 1)->negate()), PHP_EOL;
echo fc(Time\Duration::fromSeconds(1, 1)->negate()), PHP_EOL;

?>
--EXPECT--
         +0.000000000
         +0.000000001
         +1.000000001
         -0.000000001
         -1.000000001
