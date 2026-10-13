--TEST--
Io\Poll\TimerHandle: a periodic timer far behind skips the missed periods at once
--FILE--
<?php
$ctx = new Io\Poll\Context();
$ctx->add(new Io\Poll\TimerHandle(Time\Duration::fromNanoseconds(1), true), [Io\Poll\Event::Timer]);
$spent = 0;
for ($i = 0; $i < 3; $i++) {
    usleep(300000);
    $t0 = hrtime(true);
    $events = $ctx->wait(Time\Duration::fromSeconds(1));
    $spent += hrtime(true) - $t0;
    var_dump(count($events));
}
// Stepping 1ns at a time through 300ms took seconds
var_dump($spent < 500000000);
?>
--EXPECT--
int(1)
int(1)
int(1)
bool(true)
