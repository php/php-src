--TEST--
CSS Selectors - Combinators
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\HTMLDocument::createFromString(<<<HTML
<!DOCTYPE html>
<html>
    <body>
        <p>First p</p>
        <p>Second p</p>
        <img src="1.png">
        <div class="">
            <p>Third p</p>
            <img src="2.png">
            <img src="3.png">
            <div class="foo bar baz">
                <p>Fourth p</p>
            </div>
        </div>
        <article title="foo" class="bar">
            <p class="bar">Fifth p</p>
        </article>
        <table border="1">
            <colgroup>
                <col class="selected">
            </colgroup>
            <tbody>
                <tr>
                    <td>A</td>
                    <td>B</td>
                    <td>C</td>
                </tr>
            </tbody>
        </table>
    </body>
</html>
HTML);

test_helper($dom, 'nonsense');
test_helper($dom, 'p');
test_helper($dom, 'p, img');
test_helper($dom, 'body p');
test_helper($dom, 'body div p');
test_helper($dom, 'div > *');
test_helper($dom, 'div > p');
test_helper($dom, 'div > p + img');
test_helper($dom, 'div > p ~ img');
test_helper($dom, 'body > img');
test_helper($dom, 'div.bar.baz > p');
test_helper($dom, 'article[title].bar p');

try {
    test_helper($dom, 'col.selected||td');
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
--- Selector: nonsense ---
--- Selector: p ---
<p xmlns="http://www.w3.org/1999/xhtml">First p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Second p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Third p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Fourth p</p>
<p xmlns="http://www.w3.org/1999/xhtml" class="bar">Fifth p</p>
--- Selector: p, img ---
<p xmlns="http://www.w3.org/1999/xhtml">First p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Second p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Third p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Fourth p</p>
<p xmlns="http://www.w3.org/1999/xhtml" class="bar">Fifth p</p>
<img xmlns="http://www.w3.org/1999/xhtml" src="1.png" />
<img xmlns="http://www.w3.org/1999/xhtml" src="2.png" />
<img xmlns="http://www.w3.org/1999/xhtml" src="3.png" />
--- Selector: body p ---
<p xmlns="http://www.w3.org/1999/xhtml">First p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Second p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Third p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Fourth p</p>
<p xmlns="http://www.w3.org/1999/xhtml" class="bar">Fifth p</p>
--- Selector: body div p ---
<p xmlns="http://www.w3.org/1999/xhtml">Third p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Fourth p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Fourth p</p>
--- Selector: div > * ---
<p xmlns="http://www.w3.org/1999/xhtml">Third p</p>
<img xmlns="http://www.w3.org/1999/xhtml" src="2.png" />
<img xmlns="http://www.w3.org/1999/xhtml" src="3.png" />
<div xmlns="http://www.w3.org/1999/xhtml" class="foo bar baz">
                <p>Fourth p</p>
            </div>
<p xmlns="http://www.w3.org/1999/xhtml">Fourth p</p>
--- Selector: div > p ---
<p xmlns="http://www.w3.org/1999/xhtml">Third p</p>
<p xmlns="http://www.w3.org/1999/xhtml">Fourth p</p>
--- Selector: div > p + img ---
<img xmlns="http://www.w3.org/1999/xhtml" src="2.png" />
--- Selector: div > p ~ img ---
<img xmlns="http://www.w3.org/1999/xhtml" src="2.png" />
<img xmlns="http://www.w3.org/1999/xhtml" src="3.png" />
--- Selector: body > img ---
<img xmlns="http://www.w3.org/1999/xhtml" src="1.png" />
--- Selector: div.bar.baz > p ---
<p xmlns="http://www.w3.org/1999/xhtml">Fourth p</p>
--- Selector: article[title].bar p ---
<p xmlns="http://www.w3.org/1999/xhtml" class="bar">Fifth p</p>
--- Selector: col.selected||td ---
DOM\Document::querySelectorAll(): Argument #1 ($selectors) contains an unsupported selector
