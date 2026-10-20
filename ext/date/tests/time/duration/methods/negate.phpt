--TEST--
Time\Duration::negate()
--FILE--
<?php

require __DIR__ . '/../helper.inc';

echo f(Time\Duration::fromSeconds(0, 0)->negate()), PHP_EOL;
echo f(Time\Duration::fromSeconds(0, 0)->negate()->negate()), PHP_EOL;

echo f(Time\Duration::fromSeconds(1, 0)->negate()), PHP_EOL;
echo f(Time\Duration::fromSeconds(1, 0)->negate()->negate()), PHP_EOL;

echo f(Time\Duration::fromSeconds(0, 1)->negate()), PHP_EOL;
echo f(Time\Duration::fromSeconds(0, 1)->negate()->negate()), PHP_EOL;

echo f(Time\Duration::fromSeconds(1, 1)->negate()), PHP_EOL;
echo f(Time\Duration::fromSeconds(1, 1)->negate()->negate()), PHP_EOL;

?>
--EXPECT--
         +0.000000000
         +0.000000000
         -1.000000000
         +1.000000000
         -0.000000001
         +0.000000001
         -1.000000001
         +1.000000001
