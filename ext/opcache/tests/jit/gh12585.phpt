--TEST--
GH-12585: Assertion t->stack_map[t->exit_info[exit_point].stack_offset + var].type == 4
--INI--
opcache.enable=1
opcache.enable_cli=1
opcache.jit_hot_func=1
opcache.jit_hot_loop=1
opcache.jit_hot_return=1
opcache.jit_hot_side_exit=1
opcache.jit=1152
--FILE--
<?php
class MyDatePeriod extends DatePeriod
{
    public function __construct(
        DateTimeInterface $start,
        DateInterval $interval,
        int $recurrences,
        int $options = 0,
        public ?bool $myProperty = null,
    ) {
        parent::__construct($start, $interval, $recurrences, $options);
    }
}

$d = new MyDatePeriod(new DateTimeImmutable(), new DateInterval("PT5S"), 5, myProperty: true);
?>
DONE
--EXPECT--
DONE
