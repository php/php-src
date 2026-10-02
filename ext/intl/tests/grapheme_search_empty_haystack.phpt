--TEST--
Grapheme searches handle an empty haystack without ICU errors
--EXTENSIONS--
intl
--FILE--
<?php
foreach ([0, 1] as $useExceptions) {
    ini_set('intl.use_exceptions', (string) $useExceptions);
    foreach (['grapheme_strpos', 'grapheme_stripos', 'grapheme_strrpos',
              'grapheme_strripos', 'grapheme_strstr', 'grapheme_stristr'] as $function) {
        $results = [];
        foreach (['a', "\u{00E9}", "e\u{0301}", "\r\n", ''] as $needle) {
            if ($function === 'grapheme_strstr' || $function === 'grapheme_stristr') {
                $results[] = [$function('', $needle), $function('', $needle, true)];
            } else {
                $results[] = $function('', $needle);
            }
        }
        echo $function, ': ', json_encode($results), ', error: ', intl_get_error_code(), "\n";
    }
}

foreach (['grapheme_strpos', 'grapheme_stripos', 'grapheme_strrpos', 'grapheme_strripos'] as $function) {
    foreach ([-1, 1] as $offset) {
        try {
            $function('', "\u{00E9}", $offset);
        } catch (ValueError $e) {
            echo $function, '(', $offset, '): ', $e::class, "\n";
        }
    }
}
?>
--EXPECT--
grapheme_strpos: [false,false,false,false,0], error: 0
grapheme_stripos: [false,false,false,false,0], error: 0
grapheme_strrpos: [false,false,false,false,0], error: 0
grapheme_strripos: [false,false,false,false,0], error: 0
grapheme_strstr: [[false,false],[false,false],[false,false],[false,false],["",""]], error: 0
grapheme_stristr: [[false,false],[false,false],[false,false],[false,false],["",""]], error: 0
grapheme_strpos: [false,false,false,false,0], error: 0
grapheme_stripos: [false,false,false,false,0], error: 0
grapheme_strrpos: [false,false,false,false,0], error: 0
grapheme_strripos: [false,false,false,false,0], error: 0
grapheme_strstr: [[false,false],[false,false],[false,false],[false,false],["",""]], error: 0
grapheme_stristr: [[false,false],[false,false],[false,false],[false,false],["",""]], error: 0
grapheme_strpos(-1): ValueError
grapheme_strpos(1): ValueError
grapheme_stripos(-1): ValueError
grapheme_stripos(1): ValueError
grapheme_strrpos(-1): ValueError
grapheme_strrpos(1): ValueError
grapheme_strripos(-1): ValueError
grapheme_strripos(1): ValueError
