<?php
declare(strict_types=1);

require __DIR__ . '/classes.inc';

function measure(string $kind, string $metric, int $iterations): array
{
    if ($metric === 'memory') {
        // Exclude the packed array slots, but include object-store growth.
        $objects = array_fill(0, $iterations, null);
        $before = memory_get_usage(false);
        $pagesBefore = memory_get_usage(true);
        switch ($kind) {
            case 'regular':
                for ($i = 0; $i < $iterations; $i++) {
                    $objects[$i] = new RegularBookingId('booking-123');
                }
                break;
            case 'readonly':
                for ($i = 0; $i < $iterations; $i++) {
                    $objects[$i] = new ReadonlyBookingId('booking-123');
                }
                break;
            case 'value':
                for ($i = 0; $i < $iterations; $i++) {
                    $objects[$i] = new ValueBookingId('booking-123');
                }
                break;
        }
        $bytes = memory_get_usage(false) - $before;
        $pages = memory_get_usage(true) - $pagesBefore;
        return [
            'live_bytes' => $bytes,
            'allocator_pages_bytes' => $pages,
            'bytes_per_instance' => $bytes / $iterations,
            'checksum' => count($objects) + strlen($objects[$iterations - 1]->value),
        ];
    }

    $object = match ($kind) {
        'regular' => new RegularBookingId('booking-123'),
        'readonly' => new ReadonlyBookingId('booking-123'),
        'value' => new ValueBookingId('booking-123'),
    };
    $checksum = 0;
    $start = hrtime(true);
    switch ($metric) {
        case 'construct':
            switch ($kind) {
                case 'regular':
                    for ($i = 0; $i < $iterations; $i++) {
                        $object = new RegularBookingId('booking-123');
                    }
                    break;
                case 'readonly':
                    for ($i = 0; $i < $iterations; $i++) {
                        $object = new ReadonlyBookingId('booking-123');
                    }
                    break;
                case 'value':
                    for ($i = 0; $i < $iterations; $i++) {
                        $object = new ValueBookingId('booking-123');
                    }
                    break;
            }
            $checksum = strlen($object->value);
            break;
        case 'read':
            for ($i = 0; $i < $iterations; $i++) {
                $checksum += strlen($object->value);
            }
            break;
        case 'method':
            for ($i = 0; $i < $iterations; $i++) {
                $checksum += strlen($object->toString());
            }
            break;
    }
    $elapsed = hrtime(true) - $start;
    return [
        'elapsed_ns' => $elapsed,
        'ns_per_op' => $elapsed / $iterations,
        'ops_per_second' => $iterations * 1e9 / $elapsed,
        'checksum' => $checksum,
    ];
}

[$script, $kind, $metric, $count] = $argv;
$iterations = (int) $count;
if (!in_array($kind, ['regular', 'readonly', 'value'], true)
    || !in_array($metric, ['construct', 'read', 'method', 'memory'], true)
    || $iterations < 1 || (string) $iterations !== $count) {
    throw new InvalidArgumentException('Expected kind, metric and a positive iteration count');
}
if ($metric !== 'memory') {
    measure($kind, $metric, min($iterations, 100000));
}
$result = measure($kind, $metric, $iterations);
$expected = match ($metric) {
    'construct' => 11,
    'memory' => $iterations + 11,
    default => $iterations * 11,
};
if ($result['checksum'] !== $expected) {
    throw new RuntimeException('Incorrect benchmark result');
}
echo json_encode($result, JSON_THROW_ON_ERROR), "\n";
