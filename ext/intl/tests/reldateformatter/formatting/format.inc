<?php

const RELATIVE_DATE_TIME_UNITS = [
    'year' => IntlRelativeDateTimeFormatter::UNIT_YEAR,
    'quarter' => IntlRelativeDateTimeFormatter::UNIT_QUARTER,
    'month' => IntlRelativeDateTimeFormatter::UNIT_MONTH,
    'week' => IntlRelativeDateTimeFormatter::UNIT_WEEK,
    'day' => IntlRelativeDateTimeFormatter::UNIT_DAY,
    'hour' => IntlRelativeDateTimeFormatter::UNIT_HOUR,
    'minute' => IntlRelativeDateTimeFormatter::UNIT_MINUTE,
    'second' => IntlRelativeDateTimeFormatter::UNIT_SECOND,
    'sunday' => IntlRelativeDateTimeFormatter::UNIT_SUNDAY,
    'monday' => IntlRelativeDateTimeFormatter::UNIT_MONDAY,
    'tuesday' => IntlRelativeDateTimeFormatter::UNIT_TUESDAY,
    'wednesday' => IntlRelativeDateTimeFormatter::UNIT_WEDNESDAY,
    'thursday' => IntlRelativeDateTimeFormatter::UNIT_THURSDAY,
    'friday' => IntlRelativeDateTimeFormatter::UNIT_FRIDAY,
    'saturday' => IntlRelativeDateTimeFormatter::UNIT_SATURDAY,
];

const RELATIVE_DATE_TIME_OFFSETS = [
    -3, -2, -1.75, -1.5, -1, -0.5,
    0, 0.0, -0.0,
    0.5, 1, 1.5, 1.75, 2, 3,
];

/**
 * Offsets may be a list shared by all units, or a map from unit names to lists.
 * A map selects only the units it contains, preserving the shared unit order.
 */
function printRelativeDateTimeTable(
    IntlRelativeDateTimeFormatter $formatter,
    array $offsets = RELATIVE_DATE_TIME_OFFSETS,
    array $units = RELATIVE_DATE_TIME_UNITS,
): void {

    $rows = [];
    $headers = ['offset', 'format', 'formatNumeric'];

    $widths = array_map('grapheme_strlen', $headers);

    $sharedOffsets = array_is_list($offsets);

    foreach ($units as $name => $unit) {

        if (!$sharedOffsets && !array_key_exists($name, $offsets)) {
            continue;
        }

        foreach ($sharedOffsets ? $offsets : $offsets[$name] as $offset) {

            // NaN must not trigger float to string warning.
            $offsetLabel = is_nan($offset) ? 'NAN' : (string) $offset;

            $row = [
                $offsetLabel,
                (string) $formatter->format($offset, $unit),
                (string) $formatter->formatNumeric($offset, $unit),
            ];

            foreach ($row as $column => $value) {
                $widths[$column] = max($widths[$column], grapheme_strlen($value));
            }

            $rows[$name][] = $row;
        }
    }

    $printRow = static function (array $row) use ($widths): void {

        foreach ($row as $column => $value) {

            // unpadded final column to avoid trailing whitespace.
            if ($column === array_key_last($row)) {
                continue;
            }

            $space = str_repeat(' ', $widths[$column] - grapheme_strlen($value));

            $row[$column] = $column === 0
                ? $space . $value
                : $value . $space;
        }

        echo implode('  ', $row), "\n";
    };

    $fullWidth = array_sum($widths) + 2 * (count($widths) - 1);
    $separator = array_map(static fn (int $width): string => str_repeat('-', $width), $widths);

    foreach ($rows as $name => $unitRows) {

        if ($name !== array_key_first($rows)) {
            echo "\n";
        }

        echo '=== ', $name, ' ', str_repeat('=', $fullWidth - grapheme_strlen($name) - 5), "\n\n";

        $printRow($headers);
        $printRow($separator);

        foreach ($unitRows as $row) {
            $printRow($row);
        }
    }
}
