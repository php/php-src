--TEST--
.phpc: token_get_all() string path is unaffected (still requires <?php)
--FILE--
<?php
$tokens = token_get_all("<?php echo 1;");
foreach ($tokens as $t) {
    if (is_array($t)) {
        echo token_name($t[0]), "\n";
    }
}
?>
--EXPECT--
T_OPEN_TAG
T_ECHO
T_WHITESPACE
T_LNUMBER
