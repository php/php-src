--TEST--
bcdivmod() function
--EXTENSIONS--
bcmath
--INI--
bcmath.scale=0
--FILE--
<?php
require(__DIR__ . "/run_bcmath_tests_function.inc");

$dividends = ["15", "-15", "1", "-9", "14.14", "-16.60", "0.15", "-0.01"];
$divisors = array_merge($dividends, [
    "15151324141414.412312232141241",
    "-132132245132134.1515123765412",
    "141241241241241248267654747412",
    "-149143276547656984948124912",
    "0.1322135476547459213732911312",
    "-0.123912932193769965476541321",
]);

$scales = [0, 10];
foreach ($scales as $scale) {
    foreach ($dividends as $firstTerm) {
        foreach ($divisors as $secondTerm) {
            [$quot, $rem] = bcdivmod($firstTerm, $secondTerm, $scale);
            $div_ret = bcdiv($firstTerm, $secondTerm, 0);
            $mod_ret = bcmod($firstTerm, $secondTerm, $scale);

            if (bccomp($quot, $div_ret) !== 0) {
                echo "Div result is incorrect.\n";
                var_dump($firstTerm, $secondTerm, $scale, $quot, $rem, $div_ret, $mod_ret);
                echo "\n";
            }

            if (bccomp($rem, $mod_ret) !== 0) {
                echo "Mod result is incorrect.\n";
                var_dump($firstTerm, $secondTerm, $scale, $quot, $rem, $div_ret, $mod_ret);
                echo "\n";
            }
        }
    }
}
echo 'done!';
?>
--EXPECT--
done!
