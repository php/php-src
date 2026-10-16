<?php
/* Access stamps count whole seconds of the monotonic clock that hrtime() reads. Tests
 * that rely on one stamp start on a fresh second and only relax those checks when a
 * slow build crosses into the next one. */
function start_on_fresh_second(): array
{
    usleep(1030000 - intdiv(hrtime()[1], 1000));

    return hrtime();
}

function crossed_second(array $start): bool
{
    return hrtime()[0] !== $start[0];
}

/* TTLs and leases last at least their length on the same clock. */
function seconds_since(array $start): float
{
    [$sec, $nsec] = hrtime();

    return ($sec - $start[0]) + ($nsec - $start[1]) / 1e9;
}
