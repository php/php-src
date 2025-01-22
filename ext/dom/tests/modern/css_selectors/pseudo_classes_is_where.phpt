--TEST--
CSS Selectors - Pseudo classes: is/where
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<container>
    <article>
        <p>1</p>
    </article>
    <main>
        <p>2</p>
    </main>
    <div>
        <p>3</p>
    </div>
</container>
XML);

test_helper($dom, ':is(article, main) p');
test_helper($dom, ':where(article, main) p');

?>
--EXPECT--
--- Selector: :is(article, main) p ---
<p>1</p>
<p>2</p>
--- Selector: :where(article, main) p ---
<p>1</p>
<p>2</p>
