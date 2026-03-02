--TEST--
Bug #78804: Segmentation fault in Locale::filterMatches
--EXTENSIONS--
intl
--FILE--
<?php

if (Locale::filterMatches('en-US', 'und', true)) {
    echo 'Matches';
} else {
    echo 'Not matches';
}
?>
--EXPECT--
Not matches
