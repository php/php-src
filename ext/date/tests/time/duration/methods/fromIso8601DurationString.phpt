--TEST--
Time\Duration::fromIso8601DurationString()
--FILE--
<?php

require __DIR__ . '/../helper.inc';

$specifications = [
    'PT0S',
    'PT1S',
    'PT60S',
    'PT2147483647S',

    'PT0.1S',

    'PT0M',
    'PT1M',
    'PT1M1S',
    'PT1M60S',

    'PT0H',
    'PT1H',
    'PT1H1M',
    'PT1H1M1S',
    'PT1H60M',
    'PT1H60M60S',

    '',
    'P',
    'PT',
    'P1D',
    'P1W',
    'P1M',
    'P1DT0S',
    '2000-01-01T00:00:00Z',
    '2000-01-01T00:00:00Z/PT1H',
];

foreach ($specifications as $specification) {
    printf("%-25s: ", $specification);

    try {
        echo f(Time\Duration::fromIso8601DurationString($specification)), PHP_EOL;
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
PT0S                     :          +0.000000000
PT1S                     :          +1.000000000
PT60S                    :         +60.000000000
PT2147483647S            : +2147483647.000000000
PT0.1S                   : Time\TimeException: The ISO 8601 duration string could not be parsed
PT0M                     :          +0.000000000
PT1M                     :         +60.000000000
PT1M1S                   :         +61.000000000
PT1M60S                  :        +120.000000000
PT0H                     :          +0.000000000
PT1H                     :       +3600.000000000
PT1H1M                   :       +3660.000000000
PT1H1M1S                 :       +3661.000000000
PT1H60M                  :       +7200.000000000
PT1H60M60S               :       +7260.000000000
                         : Time\TimeException: The ISO 8601 duration string could not be parsed
P                        : Time\TimeException: The ISO 8601 duration string could not be parsed
PT                       : Time\TimeException: The ISO 8601 duration string could not be parsed
P1D                      : Time\TimeException: The ISO 8601 duration string may only contain the time (T) aspect
P1W                      : Time\TimeException: The ISO 8601 duration string may only contain the time (T) aspect
P1M                      : Time\TimeException: The ISO 8601 duration string may only contain the time (T) aspect
P1DT0S                   : Time\TimeException: The ISO 8601 duration string may only contain the time (T) aspect
2000-01-01T00:00:00Z     : Time\TimeException: The ISO 8601 duration string is missing the period (P) aspect
2000-01-01T00:00:00Z/PT1H: Time\TimeException: The ISO 8601 duration string may only contain the period (P) aspect
