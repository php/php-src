--TEST--
CSS Selectors - Pseudo classes: no-ops
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<html xmlns="http://www.w3.org/1999/xhtml">
    <div/>
    <a href="#foo"/>
</html>
XML);

// These will always fail or return an empty list because they are display-specific or require user interaction
test_failure($dom, ':default');
test_failure($dom, ':focus');
test_failure($dom, ':focus-visible');
test_failure($dom, ':focus-within');
test_failure($dom, ':fullscreen');
test_failure($dom, ':hover');
test_failure($dom, ':in-range');
test_failure($dom, ':indeterminate');
test_failure($dom, ':invalid');
test_failure($dom, ':out-of-range');
test_failure($dom, ':past');
test_failure($dom, ':future');
test_failure($dom, ':scope');
test_failure($dom, ':target');
test_failure($dom, ':target-within');
test_failure($dom, ':user-invalid');
test_failure($dom, ':valid');
test_failure($dom, ':visited');
test_failure($dom, ':warning');
test_failure($dom, ':local-link');
test_failure($dom, ':active');

?>
--EXPECT--
--- Selector: :default ---
Code 12 Invalid selector (Selectors. Not supported: default)
--- Selector: :focus ---
int(0)
--- Selector: :focus-visible ---
Code 12 Invalid selector (Selectors. Not supported: focus-visible)
--- Selector: :focus-within ---
Code 12 Invalid selector (Selectors. Not supported: focus-within)
--- Selector: :fullscreen ---
Code 12 Invalid selector (Selectors. Not supported: fullscreen)
--- Selector: :hover ---
int(0)
--- Selector: :in-range ---
Code 12 Invalid selector (Selectors. Not supported: in-range)
--- Selector: :indeterminate ---
Code 12 Invalid selector (Selectors. Not supported: indeterminate)
--- Selector: :invalid ---
Code 12 Invalid selector (Selectors. Not supported: invalid)
--- Selector: :out-of-range ---
Code 12 Invalid selector (Selectors. Not supported: out-of-range)
--- Selector: :past ---
Code 12 Invalid selector (Selectors. Not supported: past)
--- Selector: :future ---
Code 12 Invalid selector (Selectors. Not supported: future)
--- Selector: :scope ---
Code 12 Invalid selector (Selectors. Not supported: scope)
--- Selector: :target ---
Code 12 Invalid selector (Selectors. Not supported: target)
--- Selector: :target-within ---
Code 12 Invalid selector (Selectors. Not supported: target-within)
--- Selector: :user-invalid ---
Code 12 Invalid selector (Selectors. Not supported: user-invalid)
--- Selector: :valid ---
Code 12 Invalid selector (Selectors. Not supported: valid)
--- Selector: :visited ---
Code 12 Invalid selector (Selectors. Not supported: visited)
--- Selector: :warning ---
Code 12 Invalid selector (Selectors. Not supported: warning)
--- Selector: :local-link ---
Code 12 Invalid selector (Selectors. Not supported: local-link)
--- Selector: :active ---
int(0)
