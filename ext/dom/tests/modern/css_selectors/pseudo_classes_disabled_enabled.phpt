--TEST--
CSS Selectors - Pseudo classes: disabled/enabled
--EXTENSIONS--
dom
--FILE--
<?php

require __DIR__ . '/test_utils.inc';

$dom = DOM\XMLDocument::createFromString(<<<XML
<html xmlns="http://www.w3.org/1999/xhtml">
    <button/>
    <button xmlns="" disabled="disabled"/>
    <button disabled="disabled"/>
    <input disabled="disabled"/>
    <select disabled="disabled"/>
    <textarea disabled="disabled"/>
    <optgroup disabled="disabled"/>

    <!-- fieldset rules are complicated -->
    <fieldset disabled="disabled"/>
    <fieldset disabled="disabled">
        <fieldset id="1"/>
    </fieldset>
    <fieldset disabled="disabled">
        <!-- foo -->
        <legend>
            <div>
                <fieldset id="2"/>
            </div>
        </legend>
        <div>
            <fieldset id="3"/>
        </div>
    </fieldset>
</html>
XML);

test_helper($dom, '*:disabled');
test_helper($dom, '*:enabled');

?>
--EXPECT--
--- Selector: *:disabled ---
<button xmlns="http://www.w3.org/1999/xhtml" disabled="disabled"></button>
<input xmlns="http://www.w3.org/1999/xhtml" disabled="disabled" />
<select xmlns="http://www.w3.org/1999/xhtml" disabled="disabled"></select>
<textarea xmlns="http://www.w3.org/1999/xhtml" disabled="disabled"></textarea>
<optgroup xmlns="http://www.w3.org/1999/xhtml" disabled="disabled"></optgroup>
<fieldset xmlns="http://www.w3.org/1999/xhtml" disabled="disabled"></fieldset>
<fieldset xmlns="http://www.w3.org/1999/xhtml" disabled="disabled">
        <fieldset id="1"></fieldset>
    </fieldset>
<fieldset xmlns="http://www.w3.org/1999/xhtml" disabled="disabled">
        <!-- foo -->
        <legend>
            <div>
                <fieldset id="2"></fieldset>
            </div>
        </legend>
        <div>
            <fieldset id="3"></fieldset>
        </div>
    </fieldset>
<fieldset xmlns="http://www.w3.org/1999/xhtml" id="3"></fieldset>
--- Selector: *:enabled ---
<html xmlns="http://www.w3.org/1999/xhtml">
    <button></button>
    <button xmlns="" disabled="disabled"/>
    <button disabled="disabled"></button>
    <input disabled="disabled" />
    <select disabled="disabled"></select>
    <textarea disabled="disabled"></textarea>
    <optgroup disabled="disabled"></optgroup>

    <!-- fieldset rules are complicated -->
    <fieldset disabled="disabled"></fieldset>
    <fieldset disabled="disabled">
        <fieldset id="1"></fieldset>
    </fieldset>
    <fieldset disabled="disabled">
        <!-- foo -->
        <legend>
            <div>
                <fieldset id="2"></fieldset>
            </div>
        </legend>
        <div>
            <fieldset id="3"></fieldset>
        </div>
    </fieldset>
</html>
<button xmlns="http://www.w3.org/1999/xhtml"></button>
<button xmlns="" disabled="disabled"/>
<fieldset xmlns="http://www.w3.org/1999/xhtml" id="1"></fieldset>
<legend xmlns="http://www.w3.org/1999/xhtml">
            <div>
                <fieldset id="2"></fieldset>
            </div>
        </legend>
<div xmlns="http://www.w3.org/1999/xhtml">
                <fieldset id="2"></fieldset>
            </div>
<fieldset xmlns="http://www.w3.org/1999/xhtml" id="2"></fieldset>
<div xmlns="http://www.w3.org/1999/xhtml">
            <fieldset id="3"></fieldset>
        </div>
