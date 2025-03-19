--TEST--
NumberFormatter: currency formatting
----DESCRIPTION--
Tests NumberFormatter with various currenct-related formatters.
--EXTENSIONS--
intl
--FILE--
<?php

function ut_main() {
    $locales = [
        'ka-GE',
        'hi-IN', // No spaces between sign and the amount, crore number segmenting
        'zh-TW', // TWD has fractions, but only used in electronic transactions
    ];

    $formats = [
        'CURRENCY' => NumberFormatter::CURRENCY,
        'CURRENCY_ACCOUNTING' => NumberFormatter::CURRENCY_ACCOUNTING,
        'CURRENCY_ISO' => NumberFormatter::CURRENCY_ISO,
        'CURRENCY_PLURAL' => NumberFormatter::CURRENCY_PLURAL,
        'CASH_CURRENCY' => NumberFormatter::CASH_CURRENCY,
        'CURRENCY_STANDARD' => NumberFormatter::CURRENCY_STANDARD,
    ];

    $numbers = [0, 1, 2, 123456789.42, -123456789.42, 456.789012];

    $res_str = '';

    foreach($locales as $locale) {
        foreach ($formats as $formatLabel => $format) {
            $res_str .= "$locale: $formatLabel\n";
            foreach ($numbers as $number) {
                $fmt = ut_nfmt_create($locale, $format);
                $res_str .= "$number => " . ut_nfmt_format_currency($fmt, $number, ut_nfmt_get_symbol($fmt, NumberFormatter::INTL_CURRENCY_SYMBOL)) . "\n";
            }
            $res_str .= "\n";
        }
    }

    return $res_str;
}

include_once(__DIR__ . '/../ut_common.inc');

ut_run();
?>
--EXPECT--
ka-GE: CURRENCY
0 => 0,00 ₾
1 => 1,00 ₾
2 => 2,00 ₾
123456789.42 => 123 456 789,42 ₾
-123456789.42 => -123 456 789,42 ₾
456.789012 => 456,79 ₾

ka-GE: CURRENCY_ACCOUNTING
0 => 0,00 ₾
1 => 1,00 ₾
2 => 2,00 ₾
123456789.42 => 123 456 789,42 ₾
-123456789.42 => -123 456 789,42 ₾
456.789012 => 456,79 ₾

ka-GE: CURRENCY_ISO
0 => 0,00 GEL
1 => 1,00 GEL
2 => 2,00 GEL
123456789.42 => 123 456 789,42 GEL
-123456789.42 => -123 456 789,42 GEL
456.789012 => 456,79 GEL

ka-GE: CURRENCY_PLURAL
0 => 0,00 ქართული ლარი
1 => 1,00 ქართული ლარი
2 => 2,00 ქართული ლარი
123456789.42 => 123 456 789,42 ქართული ლარი
-123456789.42 => -123 456 789,42 ქართული ლარი
456.789012 => 456,79 ქართული ლარი

ka-GE: CASH_CURRENCY
0 => 0,00 ₾
1 => 1,00 ₾
2 => 2,00 ₾
123456789.42 => 123 456 789,42 ₾
-123456789.42 => -123 456 789,42 ₾
456.789012 => 456,79 ₾

ka-GE: CURRENCY_STANDARD
0 => 0,00 ₾
1 => 1,00 ₾
2 => 2,00 ₾
123456789.42 => 123 456 789,42 ₾
-123456789.42 => -123 456 789,42 ₾
456.789012 => 456,79 ₾

hi-IN: CURRENCY
0 => ₹0.00
1 => ₹1.00
2 => ₹2.00
123456789.42 => ₹12,34,56,789.42
-123456789.42 => -₹12,34,56,789.42
456.789012 => ₹456.79

hi-IN: CURRENCY_ACCOUNTING
0 => ₹0.00
1 => ₹1.00
2 => ₹2.00
123456789.42 => ₹12,34,56,789.42
-123456789.42 => -₹12,34,56,789.42
456.789012 => ₹456.79

hi-IN: CURRENCY_ISO
0 => INR 0.00
1 => INR 1.00
2 => INR 2.00
123456789.42 => INR 12,34,56,789.42
-123456789.42 => -INR 12,34,56,789.42
456.789012 => INR 456.79

hi-IN: CURRENCY_PLURAL
0 => 0.00 भारतीय रुपया
1 => 1.00 भारतीय रुपया
2 => 2.00 भारतीय रुपए
123456789.42 => 12,34,56,789.42 भारतीय रुपए
-123456789.42 => -12,34,56,789.42 भारतीय रुपए
456.789012 => 456.79 भारतीय रुपए

hi-IN: CASH_CURRENCY
0 => ₹0.00
1 => ₹1.00
2 => ₹2.00
123456789.42 => ₹12,34,56,789.42
-123456789.42 => -₹12,34,56,789.42
456.789012 => ₹456.79

hi-IN: CURRENCY_STANDARD
0 => ₹0.00
1 => ₹1.00
2 => ₹2.00
123456789.42 => ₹12,34,56,789.42
-123456789.42 => -₹12,34,56,789.42
456.789012 => ₹456.79

zh-TW: CURRENCY
0 => $0.00
1 => $1.00
2 => $2.00
123456789.42 => $123,456,789.42
-123456789.42 => -$123,456,789.42
456.789012 => $456.79

zh-TW: CURRENCY_ACCOUNTING
0 => $0.00
1 => $1.00
2 => $2.00
123456789.42 => $123,456,789.42
-123456789.42 => ($123,456,789.42)
456.789012 => $456.79

zh-TW: CURRENCY_ISO
0 => TWD 0.00
1 => TWD 1.00
2 => TWD 2.00
123456789.42 => TWD 123,456,789.42
-123456789.42 => -TWD 123,456,789.42
456.789012 => TWD 456.79

zh-TW: CURRENCY_PLURAL
0 => 0.00 新台幣
1 => 1.00 新台幣
2 => 2.00 新台幣
123456789.42 => 123,456,789.42 新台幣
-123456789.42 => -123,456,789.42 新台幣
456.789012 => 456.79 新台幣

zh-TW: CASH_CURRENCY
0 => $0
1 => $1
2 => $2
123456789.42 => $123,456,789
-123456789.42 => -$123,456,789
456.789012 => $457

zh-TW: CURRENCY_STANDARD
0 => $0.00
1 => $1.00
2 => $2.00
123456789.42 => $123,456,789.42
-123456789.42 => -$123,456,789.42
456.789012 => $456.79
