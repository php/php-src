--TEST--
Bug #50392 (date_create_from_format enforces 6 digits for 'u' format character)
--FILE--
<?php
date_default_timezone_set('Europe/Bratislava');

$base = '2009-03-01 18:00:00';

for ($i = 0; $i < 8; $i++) {
    $string = $base . '.' . str_repeat($i, $i);
    echo $string, "\n- ";
    $result = date_parse_from_format('Y-m-d H:i:s.u', $string);
    echo $result['fraction'] ? $result['fraction'] : 'X', "\n";
    foreach( $result['errors'] as $error ) {
        echo "- ", $error, "\n";
    }
    echo "\n";
}
?>
--EXPECT--
2009-03-01 18:00:00.
- X
- Data missing

2009-03-01 18:00:00.1
- 0.1

2009-03-01 18:00:00.22
- 0.22

2009-03-01 18:00:00.333
- 0.333

2009-03-01 18:00:00.4444
- 0.4444

2009-03-01 18:00:00.55555
- 0.55555

2009-03-01 18:00:00.666666
- 0.666666

2009-03-01 18:00:00.7777777
- 0.777777
- Trailing data
