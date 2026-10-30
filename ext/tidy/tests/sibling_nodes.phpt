--TEST--
getPreviousSibling() and getNextSibling()
--EXTENSIONS--
tidy
--FILE--
<?php

$tidy = tidy_parse_string(<<<HTML
<!DOCTYPE html>
<html>
    <body>
        <div>first</div>
        <!-- second -->
        <div>third</div>
    </body>
</html>
HTML);
$body = $tidy->body();

function format($str) {
    if (is_null($str)) return $str;
    return trim($str);
}

foreach ($body->child as $i => $child) {
    echo "=== From the perspective of child $i ===\n";
    echo "Previous: ";
    var_dump(format($child->getPreviousSibling()?->value));
    echo "Next: ";
    var_dump(format($child->getNextSibling()?->value));
}

echo "=== html element has only the doctype as sibling ===\n";
echo "Previous: ";
var_dump(format($tidy->html()->getPreviousSibling()?->value));
echo "Next: ";
var_dump(format($tidy->html()->getNextSibling()?->value));

?>
--EXPECT--
=== From the perspective of child 0 ===
Previous: NULL
Next: string(15) "<!-- second -->"
=== From the perspective of child 1 ===
Previous: string(16) "<div>first</div>"
Next: string(16) "<div>third</div>"
=== From the perspective of child 2 ===
Previous: string(15) "<!-- second -->"
Next: NULL
=== html element has only the doctype as sibling ===
Previous: string(15) "<!DOCTYPE html>"
Next: NULL
