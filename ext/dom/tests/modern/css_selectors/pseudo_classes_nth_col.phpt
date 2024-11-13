--TEST--
CSS Selectors - Pseudo classes: nth-(last-)col
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<html>
    <body>
        <table>
            <tr>
                <th>Col 1</th>
                <th>Col 2</th>
            </tr>
            <tr>
                <td>1</td>
                <td>2</td>
            </tr>
        </table>
    </body>
</html>
HTML);

test_failure($dom, ':nth-col(1)', true);
test_failure($dom, ':nth-last-col(1)', true);

?>
--EXPECT--
--- Selector: :nth-col(1) ---
Code 12 Invalid selector (Selectors. Not supported: nth-col)
--- Selector: :nth-last-col(1) ---
Code 12 Invalid selector (Selectors. Not supported: nth-last-col)
