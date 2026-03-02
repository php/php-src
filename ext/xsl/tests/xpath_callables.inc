<?php

class MyClass {
    public static function dump(string $var) {
        var_dump($var);
        return "dump: $var";
    }
}

class MyXSLTProcessor extends XSLTProcessor {
    public function registerCycle() {
        $this->registerPhpFunctions(["cycle" => array($this, "dummy")]);
    }

    public function dummy(string $var) {
        return "dummy: $var";
    }
}

function createProcessor($inputs, $class = "XSLTProcessor") {
    $xsl = new DomDocument();
    $xsl->loadXML('<?xml version="1.0" encoding="iso-8859-1" ?>
    <xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:php="http://php.net/xsl">
    <xsl:template match="//a">'
    . implode('', array_map(fn($input) => '<xsl:value-of select="php:function(' . $input . ')" />', $inputs)) .
    '</xsl:template>
    </xsl:stylesheet>');

    $proc = new $class();
    $proc->importStylesheet($xsl);
    return $proc;
}
