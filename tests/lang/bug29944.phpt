--TEST--
Bug #29944 (function defined in switch crashes PHP)
--FILE--
<?PHP
$a = 1;
$b = "1";
switch ($a) {
    case 1:
        function foo($bar) {
            if (preg_match('/\d/', $bar)) return true;
            return false;
        }
        echo foo($b);
}
?>
--EXPECT--
1
