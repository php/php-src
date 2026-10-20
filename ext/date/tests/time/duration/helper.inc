<?php

function f(Time\Duration $d, bool $pad = true, bool $plus_sign = true): string {
    $result = sprintf("%s%d.%09d", ($d->negative ? "-" : ($plus_sign ? "+" : " ")), $d->seconds, $d->nanoseconds);
    if ($pad) {
        $result = str_pad($result, 21, pad_type: STR_PAD_LEFT);
    }

    return $result;
}

function as_int(Time\Duration $d): int {
    return ($d->negative ? -1 : 1) * ($d->seconds * 1_000_000_000 + $d->nanoseconds);
}

function negate(Time\Duration $d): Time\Duration {
    return $d->negate();
}

function is_negative(Time\Duration $d): bool {
    return $d->negative;
}

/** @return Time\Duration[] */
function negate_all(
    array $d /** @param Time\Duration[] */,
): array {
    $negated = array_map(negate(...), $d);

    /* Filter out values that were not successfully negated (i.e. 0-values). */
    return array_filter($negated, is_negative(...));
}
